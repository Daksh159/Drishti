#include "strategy_engine.h"
#include <cmath>
#include <sstream>

StrategyResult StrategyEngine::recommendStrategy(const StrategyInputs& inputs) {
    StrategyResult result;
    const auto& sig = inputs.signals;

    int bullish_signals = 0;
    int bearish_signals = 0;
    int neutral_signals = 0;

    // Sentiment
    if (sig.sentiment_score > 0.3) {
        bullish_signals++;
        std::ostringstream oss;
        oss << "✓ FinBERT Sentiment: Bullish (" << sig.sentiment_score << ")";
        result.reasoning.push_back(oss.str());
    } else if (sig.sentiment_score < -0.3) {
        bearish_signals++;
        std::ostringstream oss;
        oss << "✗ FinBERT Sentiment: Bearish (" << sig.sentiment_score << ")";
        result.reasoning.push_back(oss.str());
    } else {
        neutral_signals++;
        std::ostringstream oss;
        oss << "– FinBERT Sentiment: Neutral (" << sig.sentiment_score << ")";
        result.reasoning.push_back(oss.str());
    }

    // RSI
    if (sig.rsi_last < 30) {
        bullish_signals++;
        std::ostringstream oss;
        oss << "✓ RSI: " << sig.rsi_last << " (Oversold, potential reversal)";
        result.reasoning.push_back(oss.str());
    } else if (sig.rsi_last > 70) {
        bearish_signals++;
        std::ostringstream oss;
        oss << "✗ RSI: " << sig.rsi_last << " (Overbought, potential reversal)";
        result.reasoning.push_back(oss.str());
    } else {
        neutral_signals++;
        std::ostringstream oss;
        oss << "– RSI: " << sig.rsi_last << " (Neutral zone)";
        result.reasoning.push_back(oss.str());
    }

    // MACD
    bool macd_bullish = (sig.macd_hist_last > 0) && (sig.macd_line_last > sig.macd_signal_last);
    bool macd_bearish = (sig.macd_hist_last < 0) && (sig.macd_line_last < sig.macd_signal_last);
    if (macd_bullish) {
        bullish_signals++;
        result.reasoning.push_back("✓ MACD: Bullish (Histogram positive, line above signal)");
    } else if (macd_bearish) {
        bearish_signals++;
        result.reasoning.push_back("✗ MACD: Bearish (Histogram negative, line below signal)");
    } else {
        neutral_signals++;
        result.reasoning.push_back("– MACD: Neutral");
    }

    // Price vs SMA50
    if (sig.price_vs_sma50 > 0) {
        bullish_signals++;
        result.reasoning.push_back("✓ Price > SMA50 (Bullish trend)");
    } else if (sig.price_vs_sma50 < 0) {
        bearish_signals++;
        result.reasoning.push_back("✗ Price < SMA50 (Bearish trend)");
    } else {
        neutral_signals++;
        result.reasoning.push_back("– SMA50 not available");
    }

    // Price vs SMA200
    if (sig.price_vs_sma200 > 0) {
        bullish_signals++;
        result.reasoning.push_back("✓ Price > SMA200 (Long-term bullish trend)");
    } else if (sig.price_vs_sma200 < 0) {
        bearish_signals++;
        result.reasoning.push_back("✗ Price < SMA200 (Long-term bearish trend)");
    } else {
        neutral_signals++;
        result.reasoning.push_back("– SMA200 not available");
    }

    // Determine final recommendation
    if (inputs.confidence < 50) {
        result.strategy = Strategy::NO_TRADE;
        result.reasoning.push_back("Confidence below 50% – No Trade");
        return result;
    }

    int total_signals = bullish_signals + bearish_signals + neutral_signals;
    if (bullish_signals >= 4 && inputs.confidence >= 70) {
        result.strategy = Strategy::STRONG_BUY;
    } else if (bullish_signals >= 3) {
        result.strategy = Strategy::BUY;
    } else if (bearish_signals >= 4 && inputs.confidence >=70) {
        result.strategy = Strategy::STRONG_SELL;
    } else if (bearish_signals >=3) {
        result.strategy = Strategy::SELL;
    } else {
        result.strategy = Strategy::HOLD;
    }

    return result;
}

std::string StrategyEngine::strategyToString(Strategy strategy) {
    switch (strategy) {
        case Strategy::STRONG_BUY: return "STRONG_BUY";
        case Strategy::BUY: return "BUY";
        case Strategy::HOLD: return "HOLD";
        case Strategy::SELL: return "SELL";
        case Strategy::STRONG_SELL: return "STRONG_SELL";
        case Strategy::NO_TRADE: return "NO_TRADE";
        default: return "UNKNOWN";
    }
}
