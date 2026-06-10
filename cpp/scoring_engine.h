#ifndef SCORING_ENGINE_H
#define SCORING_ENGINE_H

#include "strategy_engine.h"

struct ScoringInputs {
    SignalFactors signals;
    double volatility;
    double sharpe_ratio;
    double max_drawdown;
};

class ScoringEngine {
public:
    static double calculateTradeConfidenceScore(const ScoringInputs& inputs);
};

#endif
