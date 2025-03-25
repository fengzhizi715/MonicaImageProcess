//
// Created by Tony on 2025/3/25.
//
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

#define CONTENT_TYPE_PLAIN_TEXT "text/plain"

// 用于处理单个 HTTP 会话
class session : public std::enable_shared_from_this<session> {
public:
    session(tcp::socket socket) : socket_(std::move(socket)) {}

    void start() { do_read(); }

private:
    tcp::socket socket_;
    beast::flat_buffer buffer_;
    http::request<http::dynamic_body> req_;

    void do_read() {
        auto self = shared_from_this();
        http::async_read(socket_, buffer_, req_,
                         [self](beast::error_code ec, std::size_t bytes_transferred) {
                             if (!ec)
                                 self->handle_request();
                         });
    }

    // 根据请求路径处理请求
    void handle_request() {
        auto target = std::string(req_.target());

        if (target == "/health") {
            http::response<http::string_body> res{http::status::not_found, req_.version()};
            res.set(http::field::content_type, "text/plain");
            res.body() = "OK";
            res.prepare_payload();
            return do_write(res);
        }

        auto method = req_.method();
        // 简单路由：根据 target 分发不同的逻辑
        if (target == "/upload" && method == http::verb::post) {
            // 这里直接把上传的内容原样返回，可以在此处对图片进行处理
            http::response<http::dynamic_body> res{http::status::ok, req_.version()};
            res.set(http::field::content_type, "application/octet-stream");
            res.body() = std::move(req_.body());
            res.prepare_payload();
            do_write(res);
        }
        else {
            // 其他接口返回 404
            http::response<http::string_body> res{http::status::not_found, req_.version()};
            res.set(http::field::content_type, "text/plain");
            res.body() = "Not Found";
            res.prepare_payload();
            do_write(res);
        }
    }

    template<class Response>
    void do_write(Response& res) {
        auto self = shared_from_this();
        auto sp = std::make_shared<Response>(std::move(res));
        http::async_write(socket_, *sp,
                          [self, sp](beast::error_code ec, std::size_t) {
                              self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                          });
    }
};

// HTTP 服务器：监听指定端口，并为每个连接创建一个 session
class server {
public:
    server(net::io_context& ioc, tcp::endpoint endpoint)
            : acceptor_(ioc) {
        beast::error_code ec;
        acceptor_.open(endpoint.protocol(), ec);
        acceptor_.set_option(net::socket_base::reuse_address(true), ec);
        acceptor_.bind(endpoint, ec);
        acceptor_.listen(net::socket_base::max_listen_connections, ec);
    }

    void run() {
        do_accept();
    }

private:
    tcp::acceptor acceptor_;

    void do_accept() {
        acceptor_.async_accept(
                [this](beast::error_code ec, tcp::socket socket) {
                    if (!ec) {
                        std::make_shared<session>(std::move(socket))->start();
                    }
                    do_accept();
                });
    }
};

int main(int argc, char* argv[]) {
    // 默认配置参数
    int port = 8080;
    int numThreads = std::thread::hardware_concurrency();

    // 简单命令行参数解析（例如：./server 8000 4 表示端口8000, 4个线程）
    if (argc >= 2)
        port = std::stoi(argv[1]);
    if (argc >= 3)
        numThreads = std::stoi(argv[2]);

    net::io_context ioc{numThreads};

    tcp::endpoint endpoint{tcp::v4(), static_cast<unsigned short>(port)};
    server srv(ioc, endpoint);
    srv.run();

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads - 1; ++i) {
        threads.emplace_back([&ioc](){ ioc.run(); });
    }
    ioc.run();

    for (auto& t : threads)
        t.join();

    return 0;
}
