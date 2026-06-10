#include <iostream>
#include <string>
#include "json.hpp"
#include "analytics_engine.h"
#include "scoring_engine.h"
#include "strategy_engine.h"

using json = nlohmann::json;

int main() {
    try {
        // Read JSON from stdin
        json input;
        std::cin >> input;

        // Extract prices from input
        std::vector<double> prices = input["close"].get<std::vector<double>>();
        double sentiment_score = input.value("sentiment_score", 0.0);
        
        // Compute analytics
        auto sma = AnalyticsEngine::calculateSMA(prices, 5);
        auto ema = AnalyticsEngine::calculateEMA(prices, 5);
        auto rsi = AnalyticsEngine::calculateRSI(prices, 14);
        auto macd = AnalyticsEngine::calculateMACD(prices);
        double volatility = AnalyticsEngine::calculateVolatility(prices);
        double sharpe = AnalyticsEngine::calculateSharpeRatio(prices);
        double max_drawdown = AnalyticsEngine::calculateMaxDrawdown(prices);
        
        // Determine trend: check if last price > SMA(20)
        double trend = 0.0;
        if (prices.size() >= 20) {
            auto sma20 = AnalyticsEngine::calculateSMA(prices, 20);
            if (!sma20.empty()) {
                trend = (prices.back() > sma20.back()) ? 1.0 : -1.0;
            }
        }
        
        // Compute confidence score
        ScoringInputs scoring_inputs;
        scoring_inputs.rsi = rsi.empty() ? 50.0 : rsi.back();
        scoring_inputs.macd_histogram = macd.histogram.empty() ? 0.0 : macd.histogram.back();
        scoring_inputs.trend = trend;
        scoring_inputs.volatility = volatility;
        scoring_inputs.sharpe_ratio = sharpe;
        scoring_inputs.max_drawdown = max_drawdown;
        scoring_inputs.sentiment_score = sentiment_score;
        double confidence_score = ScoringEngine::calculateTradeConfidenceScore(scoring_inputs);
        
        // Get strategy recommendation
        StrategyInputs strategy_inputs;
        strategy_inputs.trend = trend;
        strategy_inputs.volatility = volatility;
        strategy_inputs.sentiment = sentiment_score;
        strategy_inputs.confidence = confidence_score;
        Strategy strategy = StrategyEngine::recommendStrategy(strategy_inputs);
        
        // Build output JSON
        json output;
        output["analytics"] = {
            {"sma", sma},
            {"ema", ema},
            {"rsi", rsi},
            {"macd_line", macd.macd_line},
            {"signal_line", macd.signal_line},
            {"histogram", macd.histogram},
            {"volatility", volatility},
            {"sharpe_ratio", sharpe},
            {"max_drawdown", max_drawdown}
        };
        output["trend"] = trend;
        output["confidence_score"] = confidence_score;
        output["strategy"] = StrategyEngine::strategyToString(strategy);
        
        // Write output to stdout
        std::cout << output.dump(4) << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        json error = {{"error", e.what()}};
        std::cout << error.dump(4) << std::endl;
        return 1;
    }
}
