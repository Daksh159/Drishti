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

        // Extract data from input
        std::vector<double> prices = input["close"].get<std::vector<double>>();
        std::vector<long long> volumes;
        if (input.contains("volume")) {
            volumes = input["volume"].get<std::vector<long long>>();
        }
        double sentiment_score = input.value("sentiment_score", 0.0);

        // Compute analytics
        auto sma20 = AnalyticsEngine::calculateSMA(prices, 20);
        auto sma50 = AnalyticsEngine::calculateSMA(prices, 50);
        auto sma200 = AnalyticsEngine::calculateSMA(prices, 200);
        auto ema20 = AnalyticsEngine::calculateEMA(prices, 20);
        auto rsi = AnalyticsEngine::calculateRSI(prices, 14);
        auto macd = AnalyticsEngine::calculateMACD(prices);
        auto volume_sma20 = AnalyticsEngine::calculateVolumeSMA(volumes, 20);
        auto risk_metrics = AnalyticsEngine::calculateRiskMetrics(prices);

        // Prepare signal factors
        SignalFactors signals;
        signals.sentiment_score = sentiment_score;
        signals.rsi_last = rsi.empty() ? 50.0 : rsi.back();
        signals.macd_hist_last = macd.histogram.empty() ? 0.0 : macd.histogram.back();
        signals.macd_line_last = macd.macd_line.empty() ? 0.0 : macd.macd_line.back();
        signals.macd_signal_last = macd.signal_line.empty() ? 0.0 : macd.signal_line.back();

        if (!sma50.empty()) {
            signals.price_vs_sma50 = (prices.back() > sma50.back()) ? 1.0 : -1.0;
        } else {
            signals.price_vs_sma50 = 0.0;
        }

        if (!sma200.empty()) {
            signals.price_vs_sma200 = (prices.back() > sma200.back()) ? 1.0 : -1.0;
        } else {
            signals.price_vs_sma200 = 0.0;
        }

        // Compute confidence score
        ScoringInputs scoring_inputs;
        scoring_inputs.signals = signals;
        scoring_inputs.volatility = risk_metrics.volatility;
        scoring_inputs.sharpe_ratio = risk_metrics.sharpe_ratio;
        scoring_inputs.max_drawdown = risk_metrics.max_drawdown;
        double confidence_score = ScoringEngine::calculateTradeConfidenceScore(scoring_inputs);

        // Get strategy recommendation
        StrategyInputs strategy_inputs;
        strategy_inputs.signals = signals;
        strategy_inputs.confidence = confidence_score;
        StrategyResult strategy_result = StrategyEngine::recommendStrategy(strategy_inputs);

        // Build output JSON
        json output;
        output["analytics"] = {
            {"sma20", sma20},
            {"sma50", sma50},
            {"sma200", sma200},
            {"ema20", ema20},
            {"rsi", rsi},
            {"macd_line", macd.macd_line},
            {"macd_signal_line", macd.signal_line},
            {"macd_histogram", macd.histogram},
            {"volume_sma20", volume_sma20},
            {"volatility", risk_metrics.volatility},
            {"annualized_volatility", risk_metrics.annualized_volatility},
            {"sharpe_ratio", risk_metrics.sharpe_ratio},
            {"max_drawdown", risk_metrics.max_drawdown},
            {"total_return", risk_metrics.total_return},
            {"annualized_return", risk_metrics.annualized_return},
            {"cagr", risk_metrics.cagr}
        };
        output["confidence_score"] = confidence_score;
        output["strategy"] = StrategyEngine::strategyToString(strategy_result.strategy);
        output["reasoning"] = strategy_result.reasoning;

        // Write output to stdout
        std::cout << output.dump(4) << std::endl;

        return 0;
    } catch (const std::exception& e) {
        json error = {{"error", e.what()}};
        std::cout << error.dump(4) << std::endl;
        return 1;
    }
}
