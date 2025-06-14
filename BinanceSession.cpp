#include "BinanceSession.hpp"
#include <iostream>

BinanceSession::BinanceSession(boost::asio::io_context& ioc, boost::asio::ssl::context& ctx)
    : resolver_(ioc), stream_(ioc, ctx) {}

void BinanceSession::connect_and_handshake(std::function<void()> on_connected) {
    on_connected_ = std::move(on_connected);
    resolver_.async_resolve("testnet.binance.vision", "443",
        std::bind(&BinanceSession::on_resolve, shared_from_this(),
                  std::placeholders::_1, std::placeholders::_2));
}

void BinanceSession::on_resolve(boost::system::error_code ec,
                                boost::asio::ip::tcp::resolver::results_type results) {
    if (ec) return on_error_(ec);
    boost::beast::get_lowest_layer(stream_).async_connect(results,
        std::bind(&BinanceSession::on_connect, shared_from_this(), std::placeholders::_1));
}

void BinanceSession::on_connect(boost::system::error_code ec) {
    if (ec) return on_error_(ec);
    stream_.async_handshake(boost::asio::ssl::stream_base::client,
        std::bind(&BinanceSession::on_handshake, shared_from_this(), std::placeholders::_1));
}

void BinanceSession::on_handshake(boost::system::error_code ec) {
    if (ec) return on_error_(ec);
    if (on_connected_) on_connected_();
}

void BinanceSession::send_request(boost::beast::http::request<boost::beast::http::string_body> req,
                                  std::function<void(boost::beast::http::response<boost::beast::http::string_body>)> on_response,
                                  std::function<void(boost::system::error_code)> on_error) {
    req_ = std::move(req);
    on_response_ = std::move(on_response);
    on_error_ = std::move(on_error);

    boost::beast::http::async_write(stream_, req_,
        std::bind(&BinanceSession::on_write, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void BinanceSession::on_write(boost::system::error_code ec, std::size_t) {
    if (ec) return on_error_(ec);
    boost::beast::http::async_read(stream_, buffer_, res_,
        std::bind(&BinanceSession::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void BinanceSession::on_read(boost::system::error_code ec, std::size_t) {
    if (ec) return on_error_(ec);
    on_response_(std::move(res_));
}
