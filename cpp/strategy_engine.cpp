#include "strategy_engine.h"
#include <cmath>

Strategy StrategyEngine::recommendStrategy(const StrategyInputs& inputs) {
    // Check if confidence is too low
    if (inputs.confidence < 50) {
        return Strategy::NO_TRADE;
    }

    // Check for high volatility scenarios (volatility > 20%)
    if (inputs.volatility > 0.20) {
        // High volatility + neutral trend/sentiment
        if (inputs.trend == 0 || std::abs(inputs.sentiment) < 0.3) {
            return Strategy::LONG_STRADDLE;
        }
        // High volatility + strong trend (but not super strong)
        if (std::abs(inputs.trend) == 1) {
            return Strategy::LONG_STRANGLE;
        }
    }

    // Check for low volatility (volatility < 15%)
    if (inputs.volatility < 0.15) {
        // Low volatility + neutral trend
        if (std::abs(inputs.trend) < 0.5) {
            return Strategy::IRON_CONDOR;
        }
    }

    // Directional strategies
    if (inputs.trend > 0.5 && inputs.sentiment > 0.3) {
        return Strategy::BUY_CALL;
    }
    if (inputs.trend < -0.5 && inputs.sentiment < -0.3) {
        return Strategy::BUY_PUT;
    }

    // Default
    return Strategy::NO_TRADE;
}

std::string StrategyEngine::strategyToString(Strategy strategy) {
    switch (strategy) {
        case Strategy::BUY_CALL: return "BUY_CALL";
        case Strategy::BUY_PUT: return "BUY_PUT";
        case Strategy::LONG_STRADDLE: return "LONG_STRADDLE";
        case Strategy::LONG_STRANGLE: return "LONG_STRANGLE";
        case Strategy::IRON_CONDOR: return "IRON_CONDOR";
        case Strategy::NO_TRADE: return "NO_TRADE";
        default: return "UNKNOWN";
    }
}
