#ifndef STRATEGY_ENGINE_H
#define STRATEGY_ENGINE_H

#include <string>
#include <vector>

enum class Strategy {
    STRONG_BUY,
    BUY,
    HOLD,
    SELL,
    STRONG_SELL,
    NO_TRADE
};

struct SignalFactors {
    double sentiment_score;   // -1 to +1
    double rsi_last;          // 0-100
    double macd_hist_last;    // Last MACD histogram value
    double macd_line_last;    // Last MACD line value
    double macd_signal_last;  // Last MACD signal line value
    double price_vs_sma50;    // Price > SMA50: +1, else -1, 0 if no SMA50
    double price_vs_sma200;   // Price > SMA200: +1, else -1, 0 if no SMA200
};

struct StrategyInputs {
    SignalFactors signals;
    double confidence;    // 0-100
};

struct StrategyResult {
    Strategy strategy;
    std::vector<std::string> reasoning;
};

class StrategyEngine {
public:
    static StrategyResult recommendStrategy(const StrategyInputs& inputs);
    static std::string strategyToString(Strategy strategy);
};

#endif
