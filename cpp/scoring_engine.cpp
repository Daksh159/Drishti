#include "scoring_engine.h"
#include <algorithm>

double ScoringEngine::calculateTradeConfidenceScore(const ScoringInputs& inputs) {
    // Calculate RSI Score (0-20)
    double rsi_score = 0.0;
    if (inputs.rsi > 30 && inputs.rsi < 70) {
        rsi_score = 20.0;  // Neutral zone is good
    } else if (inputs.rsi <= 30) {
        rsi_score = (30 - inputs.rsi);  // Oversold, up to 30 points
    } else {
        rsi_score = inputs.rsi - 70;  // Overbought, up to 30 points
    }
    rsi_score = std::clamp(rsi_score, 0.0, 20.0);

    // Calculate MACD Score (0-20)
    double macd_score = 0.0;
    if (inputs.macd_histogram > 0) {
        macd_score = std::min(20.0, inputs.macd_histogram * 10.0);
    } else {
        macd_score = std::max(0.0, 20.0 + inputs.macd_histogram * 10.0);
    }
    macd_score = std::clamp(macd_score, 0.0, 20.0);

    // Calculate Trend Score (0-15)
    double trend_score = (inputs.trend + 1.0) * 7.5;

    // Calculate Volatility Score (0-10)
    double vol_score = 10.0 * std::clamp(1.0 - inputs.volatility, 0.0, 1.0);

    // Calculate Sharpe Ratio Score (0-15)
    double sharpe_score = 0.0;
    if (inputs.sharpe_ratio > 2.0) {
        sharpe_score = 15.0;
    } else if (inputs.sharpe_ratio > 1.0) {
        sharpe_score = 15.0 * (inputs.sharpe_ratio - 1.0);
    }

    // Calculate Drawdown Score (0-10)
    double drawdown_score = 10.0 * std::clamp(1.0 - inputs.max_drawdown, 0.0, 1.0);

    // Calculate Sentiment Score (0-10)
    double sentiment_score = (inputs.sentiment_score + 1.0) * 5.0;

    // Total Score (sum all, clamp to 0-100)
    double total_score = rsi_score + macd_score + trend_score +
                         vol_score + sharpe_score + drawdown_score + sentiment_score;
    
    return std::clamp(total_score, 0.0, 100.0);
}
