#include "scoring_engine.h"
#include <algorithm>

double ScoringEngine::calculateTradeConfidenceScore(const ScoringInputs& inputs) {
    const auto& sig = inputs.signals;

    double score = 0.0;

    // Count bullish/bearish signals
    int bullish = 0;
    int bearish = 0;

    // Sentiment (0-20 points)
    if (sig.sentiment_score > 0.3) {
        bullish++;
        score += std::min(20.0, (sig.sentiment_score + 1.0) * 10.0);
    } else if (sig.sentiment_score < -0.3) {
        bearish++;
        score += std::max(0.0, 20.0 - (std::abs(sig.sentiment_score) * 10.0));
    } else {
        score += 10.0;
    }

    // RSI (0-20 points)
    if (sig.rsi_last > 30 && sig.rsi_last < 70) {
        score += 15.0;
    } else if (sig.rsi_last <= 30 || sig.rsi_last >= 70) {
        score += 10.0;
    } else {
        score += 10.0;
    }

    // MACD (0-20 points)
    bool macd_bullish = (sig.macd_hist_last > 0) && (sig.macd_line_last > sig.macd_signal_last);
    bool macd_bearish = (sig.macd_hist_last < 0) && (sig.macd_line_last < sig.macd_signal_last);
    if (macd_bullish) {
        bullish++;
        score += 20.0;
    } else if (macd_bearish) {
        bearish++;
        score += 5.0;
    } else {
        score += 10.0;
    }

    // Price vs SMA50 (0-15 points)
    if (sig.price_vs_sma50 > 0) {
        bullish++;
        score +=15.0;
    } else if (sig.price_vs_sma50 < 0) {
        bearish++;
        score +=5.0;
    } else {
        score +=7.0;
    }

    // Price vs SMA200 (0-15 points)
    if (sig.price_vs_sma200 >0) {
        bullish++;
        score +=15.0;
    } else if (sig.price_vs_sma200 <0) {
        bearish++;
        score +=5.0;
    } else {
        score +=7.0;
    }

    // Sharpe bonus (0-10 points)
    if (inputs.sharpe_ratio >2.0) {
        score +=10.0;
    } else if (inputs.sharpe_ratio >1.0) {
        score +=7.0;
    } else if (inputs.sharpe_ratio >0) {
        score +=3.0;
    }

    // Drawdown penalty
    if (inputs.max_drawdown > 0.30) {
        score -=10.0;
    } else if (inputs.max_drawdown >0.15) {
        score -=5.0;
    }

    return std::clamp(score, 0.0,100.0);
}
