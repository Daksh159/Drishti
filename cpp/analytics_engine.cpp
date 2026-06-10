#include "analytics_engine.h"
#include <numeric>
#include <cmath>

std::vector<double> AnalyticsEngine::calculateSMA(const std::vector<double>& prices, int period) {
    std::vector<double> sma;
    if (period <= 0 || prices.size() < static_cast<size_t>(period)) {
        return sma;
    }
    for (size_t i = period - 1; i < prices.size(); ++i) {
        double sum = std::accumulate(prices.begin() + i - period + 1, prices.begin() + i + 1, 0.0);
        sma.push_back(sum / period);
    }
    return sma;
}

std::vector<double> AnalyticsEngine::calculateEMA(const std::vector<double>& prices, int period) {
    std::vector<double> ema;
    if (period <= 0 || prices.size() < static_cast<size_t>(period)) {
        return ema;
    }
    double multiplier = 2.0 / (period + 1.0);
    auto sma = calculateSMA(prices, period);
    if (sma.empty()) {
        return ema;
    }
    ema.push_back(sma[0]);
    for (size_t i = period; i < prices.size(); ++i) {
        double ema_val = (prices[i] - ema.back()) * multiplier + ema.back();
        ema.push_back(ema_val);
    }
    return ema;
}

std::vector<double> AnalyticsEngine::calculateRSI(const std::vector<double>& prices, int period) {
    std::vector<double> rsi;
    if (period <= 0 || prices.size() < static_cast<size_t>(period + 1)) {
        return rsi;
    }
    std::vector<double> gains, losses;
    for (size_t i = 1; i < prices.size(); ++i) {
        double change = prices[i] - prices[i - 1];
        gains.push_back(change > 0 ? change : 0.0);
        losses.push_back(change < 0 ? -change : 0.0);
    }
    double avg_gain = std::accumulate(gains.begin(), gains.begin() + period, 0.0) / period;
    double avg_loss = std::accumulate(losses.begin(), losses.begin() + period, 0.0) / period;
    double rs = (avg_loss == 0.0) ? 100.0 : avg_gain / avg_loss;
    rsi.push_back(100.0 - 100.0 / (1.0 + rs));
    for (size_t i = period; i < gains.size(); ++i) {
        avg_gain = (avg_gain * (period - 1) + gains[i]) / period;
        avg_loss = (avg_loss * (period - 1) + losses[i]) / period;
        rs = (avg_loss == 0.0) ? 100.0 : avg_gain / avg_loss;
        rsi.push_back(100.0 - 100.0 / (1.0 + rs));
    }
    return rsi;
}

MACDResult AnalyticsEngine::calculateMACD(const std::vector<double>& prices, int fast_period, int slow_period, int signal_period) {
    MACDResult result;
    if (fast_period <= 0 || slow_period <= 0 || signal_period <= 0 || prices.size() < static_cast<size_t>(slow_period + signal_period)) {
        return result;
    }
    auto fast_ema = calculateEMA(prices, fast_period);
    auto slow_ema = calculateEMA(prices, slow_period);
    size_t max_size = std::min(fast_ema.size(), slow_ema.size());
    size_t macd_start = slow_ema.size() - max_size;
    for (size_t i = 0; i < max_size; ++i) {
        result.macd_line.push_back(fast_ema[i + fast_ema.size() - max_size] - slow_ema[i + macd_start]);
    }
    result.signal_line = calculateEMA(result.macd_line, signal_period);
    size_t histogram_size = std::min(result.macd_line.size(), result.signal_line.size());
    size_t histogram_start = result.macd_line.size() - histogram_size;
    for (size_t i = 0; i < histogram_size; ++i) {
        result.histogram.push_back(result.macd_line[i + histogram_start] - result.signal_line[i]);
    }
    return result;
}

double AnalyticsEngine::calculateVolatility(const std::vector<double>& prices) {
    if (prices.size() < 2) {
        return 0.0;
    }
    std::vector<double> returns;
    for (size_t i = 1; i < prices.size(); ++i) {
        returns.push_back((prices[i] - prices[i-1]) / prices[i-1]);
    }
    double mean = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    double sum_sq = 0.0;
    for (double r : returns) {
        sum_sq += (r - mean) * (r - mean);
    }
    return std::sqrt(sum_sq / returns.size());
}

double AnalyticsEngine::calculateSharpeRatio(const std::vector<double>& prices, double risk_free_rate, int periods_per_year) {
    double volatility = calculateVolatility(prices);
    if (volatility == 0.0 || prices.size() < 2) {
        return 0.0;
    }
    std::vector<double> returns;
    for (size_t i = 1; i < prices.size(); ++i) {
        returns.push_back((prices[i] - prices[i-1]) / prices[i-1]);
    }
    double mean_return = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    double annual_return = (1.0 + mean_return) * periods_per_year - 1.0;
    double annual_volatility = volatility * std::sqrt(periods_per_year);
    return (annual_return - risk_free_rate) / annual_volatility;
}

double AnalyticsEngine::calculateMaxDrawdown(const std::vector<double>& prices) {
    if (prices.empty()) {
        return 0.0;
    }
    double peak = prices[0];
    double max_drawdown = 0.0;
    for (double price : prices) {
        if (price > peak) {
            peak = price;
        }
        double drawdown = (peak - price) / peak;
        if (drawdown > max_drawdown) {
            max_drawdown = drawdown;
        }
    }
    return max_drawdown;
}
