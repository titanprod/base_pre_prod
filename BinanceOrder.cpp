#include "BinanceOrder.hpp"
#include <openssl/hmac.h>
#include <chrono>
#include <boost/beast/http.hpp>
#include <iostream>

using namespace boost::beast;

std::string BinanceOrder::hmac_sha256(const std::string& key, const std::string& data) {
    unsigned char* digest = HMAC(EVP_sha256(),
                                 key.c_str(), key.length(),
                                 reinterpret_cast<const unsigned char*>(data.c_str()), data.length(), NULL, NULL);
    char mdString[65];
    for (int i = 0; i < 32; ++i)
        sprintf(&mdString[i * 2], "%02x", digest[i]);
    return std::string(mdString);
}

std::string BinanceOrder::get_timestamp() {
    using namespace std::chrono;
    return std::to_string(duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()).count());
}

BinanceOrder::BinanceOrder(std::shared_ptr<BinanceSession> session,
                           const std::string& api_key,
                           const std::string& secret_key,
                           const std::string& symbol,
                           const std::string& side,
                           const std::string& price,
                           const std::string& quantity)
    : session_(session),
      api_key_(api_key),
      secret_key_(secret_key),
      symbol_(symbol),
      side_(side),
      price_(price),
      quantity_(quantity) {}

void BinanceOrder::execute() {
    std::string timestamp = get_timestamp();
    std::string query = "symbol=" + symbol_ +
                        "&side=" + side_ +
                        "&type=LIMIT&timeInForce=GTC" +
                        "&quantity=" + quantity_ +
                        "&price=" + price_ +
                        "&timestamp=" + timestamp;

    std::string signature = hmac_sha256(secret_key_, query);
    query += "&signature=" + signature;
    std::string target = "/api/v3/order?" + query;

    http::request<http::string_body> req{http::verb::post, target, 11};
    req.set(http::field::host, "testnet.binance.vision");
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set("X-MBX-APIKEY", api_key_);
    req.body() = "";
    req.prepare_payload();

    session_->send_request(
        std::move(req),
        [side = side_, price = price_](http::response<http::string_body> res) {
            std::cout << "[" << side << " at " << price << "] Order Response:\n" << res << "\n";
        },
        [side = side_, price = price_](boost::system::error_code ec) {
            std::cerr << "Error placing " << side << " order at " << price << ": " << ec.message() << "\n";
        });
}

void BinanceOrder::cancel(const std::string& order_id) {
    std::string timestamp = get_timestamp();
    std::string query = "symbol=" + symbol_ +
                        "&orderId=" + order_id +
                        "&timestamp=" + timestamp;

    std::string signature = hmac_sha256(secret_key_, query);
    query += "&signature=" + signature;
    std::string target = "/api/v3/order?" + query;

    http::request<http::string_body> req{http::verb::delete_, target, 11};
    req.set(http::field::host, "testnet.binance.vision");
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set("X-MBX-APIKEY", api_key_);
    req.body() = "";
    req.prepare_payload();

    session_->send_request(
        std::move(req),
        [order_id](http::response<http::string_body> res) {
            std::cout << "[CANCEL] Order " << order_id << " Response:\n" << res << "\n";
        },
        [order_id](boost::system::error_code ec) {
            std::cerr << "Error canceling order " << order_id << ": " << ec.message() << "\n";
        });
}
