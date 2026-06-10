#ifndef SCORING_ENGINE_H
#define SCORING_ENGINE_H

struct ScoringInputs {
    double rsi;
    double macd_histogram;
    double trend;  // +1 for bullish, -1 for bearish, 0 for neutral
    double volatility;
    double sharpe_ratio;
    double max_drawdown;
    double sentiment_score;  // -1 to +1
};

class ScoringEngine {
public:
    static double calculateTradeConfidenceScore(const ScoringInputs& inputs);
};

#endif
