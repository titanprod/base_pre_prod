#ifndef BINANCE_ORDER_HPP
#define BINANCE_ORDER_HPP

#include "BinanceSession.hpp"
#include <string>
#include <memory>

class BinanceOrder {
public:
    BinanceOrder(std::shared_ptr<BinanceSession> session,
                 const std::string& api_key,
                 const std::string& secret_key,
                 const std::string& symbol,
                 const std::string& side,
                 const std::string& price,
                 const std::string& quantity);

    void execute();
    void cancel(const std::string& order_id);

private:
    std::shared_ptr<BinanceSession> session_;
    std::string api_key_;
    std::string secret_key_;
    std::string symbol_;
    std::string side_;
    std::string price_;
    std::string quantity_;

    std::string hmac_sha256(const std::string& key, const std::string& data);
    std::string get_timestamp();
};

#endif
