#ifndef ANALYTICS_ENGINE_H
#define ANALYTICS_ENGINE_H

#include <vector>
#include <utility>

struct MACDResult {
    std::vector<double> macd_line;
    std::vector<double> signal_line;
    std::vector<double> histogram;
};

class AnalyticsEngine {
public:
    static std::vector<double> calculateSMA(const std::vector<double>& prices, int period);
    static std::vector<double> calculateEMA(const std::vector<double>& prices, int period);
    static std::vector<double> calculateRSI(const std::vector<double>& prices, int period = 14);
    static MACDResult calculateMACD(const std::vector<double>& prices, int fast_period = 12, int slow_period = 26, int signal_period = 9);
    static double calculateVolatility(const std::vector<double>& prices);
    static double calculateSharpeRatio(const std::vector<double>& prices, double risk_free_rate = 0.02, int periods_per_year = 252);
    static double calculateMaxDrawdown(const std::vector<double>& prices);
};

#endif
