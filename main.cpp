#include "BinanceSession.hpp"
#include "BinanceOrder.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <memory>

int main() {
    const std::string API_KEY = "your_api_key";
    const std::string SECRET_KEY = "your_secret_key";

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
        cancel_session, API_KEY, SECRET_KEY, "BTCUSDC", "", "", ""); // No price or quantity needed for cancel

    buy_session->connect_and_handshake([buy_order]() {
        buy_order->execute();
    });

    sell_session->connect_and_handshake([sell_order]() {
        sell_order->execute();
    });

    cancel_session->connect_and_handshake([cancel_order]() {
        cancel_order->cancel("1957514");
    });

    ioc.run();
    return 0;
}
