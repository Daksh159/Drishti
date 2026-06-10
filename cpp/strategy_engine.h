#ifndef STRATEGY_ENGINE_H
#define STRATEGY_ENGINE_H

#include <string>

enum class Strategy {
    BUY_CALL,
    BUY_PUT,
    LONG_STRADDLE,
    LONG_STRANGLE,
    IRON_CONDOR,
    NO_TRADE
};

struct StrategyInputs {
    double trend;         // +1 bullish, -1 bearish, 0 neutral
    double volatility;    // as decimal
    double sentiment;     // -1 to +1
    double confidence;    // 0-100
};

class StrategyEngine {
public:
    static Strategy recommendStrategy(const StrategyInputs& inputs);
    static std::string strategyToString(Strategy strategy);
};

#endif
