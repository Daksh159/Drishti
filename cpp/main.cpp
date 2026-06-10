#include <iostream>
#include <iomanip>
#include "analytics_engine.h"
#include "scoring_engine.h"
#include "strategy_engine.h"

void printVector(const std::vector<double>& vec, const std::string& label) {
    std::cout << label << ": ";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << std::fixed << std::setprecision(2) << vec[i];
        if (i < vec.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "=== Analytics Engine Tests ===" << std::endl << std::endl;

    // Test data: Simple linear sequence (longer)
    std::vector<double> test_prices;
    for (int i = 1; i <= 50; ++i) {
        test_prices.push_back(i * 1.0);
    }

    // Test SMA
    auto sma = AnalyticsEngine::calculateSMA(test_prices, 5);
    std::cout << "--- SMA Test (period=5) ---" << std::endl;
    printVector(sma, "SMA");
    std::cout << std::endl;

    // Test EMA
    auto ema = AnalyticsEngine::calculateEMA(test_prices, 5);
    std::cout << "--- EMA Test (period=5) ---" << std::endl;
    printVector(ema, "EMA");
    std::cout << std::endl;

    // Test RSI
    auto rsi = AnalyticsEngine::calculateRSI(test_prices, 14);
    std::cout << "--- RSI Test (period=14) ---" << std::endl;
    printVector(rsi, "RSI");
    std::cout << std::endl;

    // Test MACD
    auto macd = AnalyticsEngine::calculateMACD(test_prices, 12, 26, 9);
    std::cout << "--- MACD Test ---" << std::endl;
    printVector(macd.macd_line, "MACD Line");
    printVector(macd.signal_line, "Signal Line");
    printVector(macd.histogram, "Histogram");
    std::cout << std::endl;

    // Test Risk Metrics
    std::cout << "--- Risk Metrics Tests ---" << std::endl;

    // Volatility test: Linear series (should have constant returns)
    double volatility = AnalyticsEngine::calculateVolatility(test_prices);
    std::cout << "Volatility (linear series): " << std::fixed << std::setprecision(6) << volatility << std::endl;

    // Sharpe Ratio test
    double sharpe = AnalyticsEngine::calculateSharpeRatio(test_prices, 0.02, 252);
    std::cout << "Sharpe Ratio (linear series): " << std::fixed << std::setprecision(4) << sharpe << std::endl;

    // Max Drawdown test: First create a series with drawdown
    std::vector<double> drawdown_prices = {100, 110, 105, 95, 100, 108, 90, 120};
    double max_dd = AnalyticsEngine::calculateMaxDrawdown(drawdown_prices);
    std::cout << "Max Drawdown (drawdown test series): " << std::fixed << std::setprecision(2) << (max_dd * 100) << "%" << std::endl;

    // Test Scoring Engine
    std::cout << std::endl << "--- Scoring Engine Tests ---" << std::endl;

    // Test 1: Bullish scenario
    ScoringInputs bullish_inputs;
    bullish_inputs.rsi = 50.0;
    bullish_inputs.macd_histogram = 2.0;
    bullish_inputs.trend = 1.0;
    bullish_inputs.volatility = 0.10;
    bullish_inputs.sharpe_ratio = 2.5;
    bullish_inputs.max_drawdown = 0.05;
    bullish_inputs.sentiment_score = 0.8;
    double bullish_score = ScoringEngine::calculateTradeConfidenceScore(bullish_inputs);
    std::cout << "Bullish scenario score: " << std::fixed << std::setprecision(1) << bullish_score << std::endl;

    // Test 2: Bearish scenario
    ScoringInputs bearish_inputs;
    bearish_inputs.rsi = 80.0;
    bearish_inputs.macd_histogram = -2.0;
    bearish_inputs.trend = -1.0;
    bearish_inputs.volatility = 0.30;
    bearish_inputs.sharpe_ratio = 0.5;
    bearish_inputs.max_drawdown = 0.25;
    bearish_inputs.sentiment_score = -0.8;
    double bearish_score = ScoringEngine::calculateTradeConfidenceScore(bearish_inputs);
    std::cout << "Bearish scenario score: " << std::fixed << std::setprecision(1) << bearish_score << std::endl;

    // Test Strategy Engine
    std::cout << std::endl << "--- Strategy Engine Tests ---" << std::endl;

    // Test 1: Bullish call scenario
    StrategyInputs call_inputs;
    call_inputs.trend = 1.0;
    call_inputs.volatility = 0.15;
    call_inputs.sentiment = 0.8;
    call_inputs.confidence = 85.0;
    Strategy call_strategy = StrategyEngine::recommendStrategy(call_inputs);
    std::cout << "Bullish scenario strategy: " << StrategyEngine::strategyToString(call_strategy) << std::endl;

    // Test 2: Bearish put scenario
    StrategyInputs put_inputs;
    put_inputs.trend = -1.0;
    put_inputs.volatility = 0.18;
    put_inputs.sentiment = -0.7;
    put_inputs.confidence = 75.0;
    Strategy put_strategy = StrategyEngine::recommendStrategy(put_inputs);
    std::cout << "Bearish scenario strategy: " << StrategyEngine::strategyToString(put_strategy) << std::endl;

    // Test 3: High volatility neutral (straddle)
    StrategyInputs straddle_inputs;
    straddle_inputs.trend = 0.0;
    straddle_inputs.volatility = 0.25;
    straddle_inputs.sentiment = 0.1;
    straddle_inputs.confidence = 70.0;
    Strategy straddle_strategy = StrategyEngine::recommendStrategy(straddle_inputs);
    std::cout << "High vol neutral strategy: " << StrategyEngine::strategyToString(straddle_strategy) << std::endl;

    // Test 4: Low volatility neutral (iron condor)
    StrategyInputs condor_inputs;
    condor_inputs.trend = 0.2;
    condor_inputs.volatility = 0.10;
    condor_inputs.sentiment = 0.0;
    condor_inputs.confidence = 65.0;
    Strategy condor_strategy = StrategyEngine::recommendStrategy(condor_inputs);
    std::cout << "Low vol neutral strategy: " << StrategyEngine::strategyToString(condor_strategy) << std::endl;

    // Test 5: Low confidence (no trade)
    StrategyInputs no_trade_inputs;
    no_trade_inputs.trend = 1.0;
    no_trade_inputs.volatility = 0.15;
    no_trade_inputs.sentiment = 0.5;
    no_trade_inputs.confidence = 40.0;
    Strategy no_trade_strategy = StrategyEngine::recommendStrategy(no_trade_inputs);
    std::cout << "Low confidence strategy: " << StrategyEngine::strategyToString(no_trade_strategy) << std::endl;

    return 0;
}
