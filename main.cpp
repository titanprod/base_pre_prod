#include "BinanceSession.hpp"
#include "BinanceOrder.hpp"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <memory>
#include <iostream>

int main() {
    const std::string API_KEY = "hyDGVZTKh3BYzLBrW7rwLbBJJD8kSck2BYnZ0lol9id1PPwLA5oqquF4UUqYHPmS";
    const std::string SECRET_KEY = "3FK38LZuhT8j2wBBLZTMQx4VXiBz1M3iiVTXBSaX4zUkwAi1uEFkaglsFlan3wyJ";

    boost::asio::io_context ioc;
    boost::asio::ssl::context ssl_ctx(boost::asio::ssl::context::tlsv12_client);

    auto buy_session = std::make_shared<BinanceSession>(ioc, ssl_ctx);
    auto sell_session = std::make_shared<BinanceSession>(ioc, ssl_ctx);
    auto cancel_session = std::make_shared<BinanceSession>(ioc, ssl_ctx);

    auto buy_order = std::make_shared<BinanceOrder>(
        buy_session, API_KEY, SECRET_KEY, "BTCUSDC", "BUY", "106500", "0.001");

    auto sell_order = std::make_shared<BinanceOrder>(
        sell_session, API_KEY, SECRET_KEY, "BTCUSDC", "SELL", "107800", "0.001");

    auto cancel_order = std::make_shared<BinanceOrder>(
        cancel_session, API_KEY, SECRET_KEY, "BTCUSDC", "", "", ""); // Empty for cancel

    // Setup buy session
    buy_session->connect_and_handshake(
        [buy_order]() {
            std::cout << "[BUY] Connected. Placing order...\n";
            buy_order->execute();
        },
        [](boost::system::error_code ec) {
            std::cerr << "[BUY] Error: " << ec.message() << "\n";
        }
    );

    // Setup sell session
    sell_session->connect_and_handshake(
        [sell_order]() {
            std::cout << "[SELL] Connected. Placing order...\n";
            sell_order->execute();
        },
        [](boost::system::error_code ec) {
            std::cerr << "[SELL] Error: " << ec.message() << "\n";
        }
    );

    // Setup cancel session
    cancel_session->connect_and_handshake(
        [cancel_order]() {
            std::cout << "[CANCEL] Connected. Canceling order...\n";
            cancel_order->cancel("1957514");
        },
        [](boost::system::error_code ec) {
            std::cerr << "[CANCEL] Error: " << ec.message() << "\n";
        }
    );

    ioc.run();
    return 0;
}
