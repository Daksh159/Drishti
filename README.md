# Drishti - C++ Quantitative Trade Decision Engine

A production-quality automated trading decision system that analyzes stock market data, financial news sentiment, and risk metrics to generate a proprietary **Trade Confidence Score (0-100)** and recommend specific options trading strategies.

---

## 📋 Table of Contents
1. [Project Overview](#-project-overview)
2. [Why I Built This](#-why-i-built-this)
3. [What Makes Drishti Unique](#-what-makes-drishti-unique)
4. [Technical Architecture](#-technical-architecture)
5. [How It Works](#-how-it-works-step-by-step-flow)
6. [Technologies Used](#-technologies-used)
7. [Key Skills Demonstrated](#-key-skills-demonstrated)
8. [Getting Started](#-getting-started)
9. [Project Structure](#-project-structure)

---

## 📈 Project Overview

**Drishti** is your personal "stock analyst in a box" - it does all the technical analysis, sentiment analysis, and risk calculations automatically, then tells you:
- How confident it is in a trade (0-100)
- What specific strategy to use (BUY_CALL, BUY_PUT, etc.)

It combines three powerful layers:
1. **Data Collection** (Python + yfinance)
2. **AI Sentiment Analysis** (FinBERT)
3. **C++ Core Engine** (Analytics + Scoring + Strategy)

---

## 🎯 Why I Built This

I created Drishti to solve 3 big problems in trading:

1. **Information Overload**: Traders have to look at 10+ different indicators, news headlines, and charts - it's overwhelming!
2. **Emotional Bias**: People often make bad decisions because of fear or greed, not data
3. **Slow Analysis**: By the time you manually analyze everything, the opportunity might be gone

Drishti solves these by:
- ✅ Combining **technical analysis**, **sentiment analysis**, and **risk management** in one system
- ✅ Using **C++ for speed** (critical for real-time trading)
- ✅ Removing emotional bias by making data-driven decisions
- ✅ Generating clear, actionable recommendations

---

## 🚀 What Makes Drishti Unique

This isn't just another technical indicator script - here's what makes it special:

### 1. Hybrid C++/Python Architecture
- **Core logic in C++**: All heavy calculations (analytics, scoring, strategy) for **blazing fast performance**
- **Data layer in Python**: Leverages Python's ecosystem for data fetching (yfinance) and AI/ML (FinBERT)
- **JSON communication**: Clean, simple inter-process communication via JSON stdin/stdout

### 2. Proprietary Trade Confidence Score
Custom scoring algorithm combining:
- **40% Technical Indicators** (SMA, EMA, RSI, MACD)
- **30% Risk Metrics** (Volatility, Sharpe Ratio, Max Drawdown)
- **30% News Sentiment** (via FinBERT AI)

Single, easy-to-understand number from 0-100!

### 3. Multiple Options Trading Strategies
Instead of just "buy" or "sell", recommends specific options strategies:
- **BUY_CALL**: Bullish, limited risk
- **BUY_PUT**: Bearish, limited risk  
- **LONG_STRADDLE**: Expecting big move, direction unknown
- **LONG_STRANGLE**: Expecting big move, cheaper than straddle
- **IRON_CONDOR**: Neutral, income strategy
- **NO_TRADE**: Wait for better opportunity

### 4. Streamlit Dashboard for Visualization
Beautiful, interactive web dashboard showing:
- Price charts with candlesticks
- Technical indicators (RSI, MACD)
- All key metrics in one place
- Real-time analysis with one click

---
## OUTPUT

<img width="1217" height="813" alt="image" src="https://github.com/user-attachments/assets/f86c0263-0098-4c04-be30-3fc9deecd093" />

<img width="1365" height="824" alt="image" src="https://github.com/user-attachments/assets/4699cda0-0ff0-4854-99b9-f24b881c5d7c" />


---

## 🏗️ Technical Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                      Streamlit Dashboard                        │
│                    (User Interface Layer)                       │
└─────────────────────────────┬───────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                    Python Integration Layer                     │
│  ┌─────────────────┐  ┌──────────────────┐  ┌─────────────────┐ │
│  │  Data Fetcher   │  │ Sentiment Engine │  │   Pipeline      │ │
│  │   (yfinance)    │  │   (FinBERT)      │  │   Orchestrator  │ │
│  └─────────────────┘  └──────────────────┘  └────────┬────────┘ │
└───────────────────────────────────────────────────────┼──────────┘
                                                        │ JSON
                                                        ▼
┌─────────────────────────────────────────────────────────────────┐
│                    C++ Core Engine Layer                        │
│  ┌─────────────────┐  ┌──────────────────┐  ┌─────────────────┐ │
│  │ Analytics Engine│  │  Scoring Engine  │  │ Strategy Engine │ │
│  │  (Indicators)   │  │  (Confidence)    │  │  (Strategies)   │ │
│  └─────────────────┘  └──────────────────┘  └─────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🔄 How It Works (Step-by-Step Flow)

### Step 1: Fetch Market Data
- Python uses **yfinance** to get real OHLCV (Open, High, Low, Close, Volume) data for any stock
- Has fallback to synthetic data if yfinance has issues
- Fixed timezone cache bug with monkey-patching!

### Step 2: Analyze News Sentiment
- Uses **FinBERT** (a pre-trained AI model for financial text)
- Enter a headline like "Apple reports record revenue"
- Returns: **bullish/bearish/neutral** + a score from 0-1

### Step 3: C++ Engine Crunches Numbers
Python sends data to C++ via JSON, then C++ does:

#### 3.1 Analytics Engine
Calculates all technical indicators:
- **SMA** (Simple Moving Average): 20-day and 50-day
- **EMA** (Exponential Moving Average): More weight to recent prices
- **RSI** (Relative Strength Index): 0-100, identifies overbought/oversold conditions
- **MACD** (Moving Average Convergence Divergence): Trend and momentum
- **Volatility**: Price movement magnitude
- **Sharpe Ratio**: Risk-adjusted return
- **Max Drawdown**: Worst peak-to-trough loss

#### 3.2 Scoring Engine
Calculates the proprietary **Trade Confidence Score**:
```
Score = (0.4 × Technical Score) + (0.3 × Risk Score) + (0.3 × Sentiment Score)
```

#### 3.3 Strategy Engine
Recommends strategy based on score and market conditions:
- High confidence (>70) + bullish → BUY_CALL
- High confidence (>70) + bearish → BUY_PUT
- Medium confidence (50-70) + high volatility → LONG_STRADDLE/STRANGLE
- Low confidence (<50) → NO_TRADE

### Step 4: Show Results in Dashboard
Beautiful Streamlit UI displays:
- Key metrics in cards
- Interactive price chart
- RSI and MACD charts
- Clear strategy recommendation

---

## 🛠️ Technologies Used

| Layer | Technologies |
|-------|--------------|
| **Frontend/Dashboard** | Streamlit, Plotly, Pandas |
| **Data Collection** | Python, yfinance |
| **AI/Sentiment** | Hugging Face Transformers, FinBERT, PyTorch |
| **Core Engine** | C++17, nlohmann/json |
| **Integration** | JSON, stdin/stdout communication |

---

## 💼 Key Skills Demonstrated

This project showcases proficiency in:

1. **Multi-language programming**: C++ and Python integration
2. **Financial domain knowledge**: Technical analysis, risk metrics, options strategies
3. **AI/ML integration**: Using pre-trained models (FinBERT)
4. **Performance optimization**: C++ for compute-heavy tasks
5. **API design**: JSON-based microservice architecture
6. **UI/UX**: Building interactive dashboards
7. **Problem solving**: Debugging yfinance cache issues, library conflicts, etc.
8. **End-to-end development**: From data collection to production UI

---

## 🚀 Getting Started

### Prerequisites
- Python 3.12+
- C++17 compiler (MSVC, GCC, or Clang)
- CMake (optional)

### Installation

1. **Clone the repository**:
   ```bash
   git clone <repo-url>
   cd Drishti
   ```

2. **Install Python dependencies**:
   ```bash
   cd python
   pip install -r requirements.txt
   ```

3. **Compile C++ engine**:
   ```bash
   cd ../cpp
   # Compile drishti.exe (or drishti on Linux/macOS)
   g++ -std=c++17 drishti.cpp analytics_engine.cpp scoring_engine.cpp strategy_engine.cpp -o drishti.exe
   ```

4. **Run the dashboard**:
   ```bash
   cd ../python
   streamlit run dashboard.py
   ```

5. **Open your browser**: Navigate to `http://localhost:8501`

### Test the Pipeline
```bash
cd python
python pipeline.py
```

---

## 📁 Project Structure

```
Drishti/
├── cpp/                          # C++ Core Engine
│   ├── analytics_engine.h/cpp    # Technical indicators & risk metrics
│   ├── scoring_engine.h/cpp      # Trade Confidence Score calculation
│   ├── strategy_engine.h/cpp     # Trading strategy recommendations
│   ├── drishti.cpp               # JSON I/O endpoint
│   ├── main.cpp                  # Test suite
│   └── json.hpp                  # nlohmann/json library
├── python/                       # Python Integration Layer
│   ├── data_fetcher.py           # yfinance data collection
│   ├── sentiment_engine.py       # FinBERT sentiment analysis
│   ├── pipeline.py               # End-to-end orchestration
│   └── dashboard.py              # Streamlit UI
├── output/                       # Output data files
│   ├── aapl_ohlcv.json
│   ├── aapl_results.json
│   └── sentiment_results.json
└── README.md                     # This file
```

---

## 📝 License
MIT License - feel free to use this project for learning and portfolio purposes!
