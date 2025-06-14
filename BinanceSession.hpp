#ifndef BINANCE_SESSION_HPP
#define BINANCE_SESSION_HPP

#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <functional>

class BinanceSession : public std::enable_shared_from_this<BinanceSession> {
public:
    BinanceSession(boost::asio::io_context& ioc, boost::asio::ssl::context& ctx);

    void connect_and_handshake(std::function<void()> on_connected);

    void send_request(boost::beast::http::request<boost::beast::http::string_body> req,
                      std::function<void(boost::beast::http::response<boost::beast::http::string_body>)> on_response,
                      std::function<void(boost::system::error_code)> on_error);

private:
    void on_resolve(boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type results);
    void on_connect(boost::system::error_code ec);
    void on_handshake(boost::system::error_code ec);
    void on_write(boost::system::error_code ec, std::size_t bytes_transferred);
    void on_read(boost::system::error_code ec, std::size_t bytes_transferred);

    boost::asio::ip::tcp::resolver resolver_;
    boost::beast::ssl_stream<boost::beast::tcp_stream> stream_;
    boost::beast::flat_buffer buffer_;
    boost::beast::http::request<boost::beast::http::string_body> req_;
    boost::beast::http::response<boost::beast::http::string_body> res_;
    std::function<void(boost::beast::http::response<boost::beast::http::string_body>)> on_response_;
    std::function<void(boost::system::error_code)> on_error_;
    std::function<void()> on_connected_;
};

#endif
