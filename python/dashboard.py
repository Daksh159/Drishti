
# First, set environment variables to reduce warnings
import os
os.environ["TRANSFORMERS_NO_ADVISORY_WARNINGS"] = "1"
os.environ["TRANSFORMERS_VERBOSITY"] = "error"
os.environ["TF_CPP_MIN_LOG_LEVEL"] = "3"
import warnings
warnings.filterwarnings("ignore")
import logging

# Suppress specific libraries' logs, NOT Streamlit!
logging.getLogger("transformers").setLevel(logging.ERROR)
logging.getLogger("torch").setLevel(logging.ERROR)
logging.getLogger("yfinance").setLevel(logging.ERROR)

# Import Streamlit FIRST (it needs stdout/stderr!)
import streamlit as st
import json
import pandas as pd
import plotly.graph_objects as go

# Now handle yfinance timezone cache issue
import yfinance.base as yfbase
def dummy_get_ticker_tz(self, timeout=10):
    import datetime
    return datetime.timezone.utc
yfbase.TickerBase._get_ticker_tz = dummy_get_ticker_tz

# Now import our modules
from data_fetcher import fetch_ohlcv
from sentiment_engine import SentimentEngine
from pipeline import run_cpp_engine

st.set_page_config(
    page_title="Drishti - Trade Decision Engine",
    page_icon="📈",
    layout="wide",
    initial_sidebar_state="expanded"
)

st.title("📈 Drishti - C++ Quantitative Trade Decision Engine")
st.markdown("---")

# Sidebar: Configuration
st.sidebar.header("⚙️ Configuration")
ticker = st.sidebar.text_input("Stock Ticker", "AAPL")
period = st.sidebar.selectbox("Time Period", ["1mo", "3mo", "6mo", "1y", "2y", "5y"], index=3)
interval = st.sidebar.selectbox("Data Interval", ["1d", "1wk", "1mo"], index=0)
sentiment_headline = st.sidebar.text_input(
    "Sentiment Headline",
    "Apple reports record quarterly revenue, beats earnings estimates"
)

if st.sidebar.button("Run Analysis 🚀"):
    try:
        with st.spinner("Running analysis... (1/3 Fetching data)"):
            # Step 1: Fetch data
            market_data = fetch_ohlcv(ticker, period, interval)
            df = pd.DataFrame({
                "Date": pd.to_datetime(market_data["dates"]),
                "Open": market_data["open"],
                "High": market_data["high"],
                "Low": market_data["low"],
                "Close": market_data["close"],
                "Volume": market_data["volume"]
            }).set_index("Date")
        
        with st.spinner("Analyzing sentiment... (2/3)"):
            # Step 2: Analyze sentiment
            sentiment_engine = SentimentEngine()
            sentiment = sentiment_engine.analyze(sentiment_headline)
        
        with st.spinner("Running C++ engine... (3/3)"):
            # Step 3: Run C++ engine
            project_root = os.path.dirname(os.path.dirname(__file__))
            cpp_exe = os.path.join(project_root, "cpp", "drishti.exe")
            cpp_input = {
                "close": market_data["close"],
                "sentiment_score": sentiment["score"]
            }
            cpp_results = run_cpp_engine(cpp_input, cpp_exe)
            
            if not cpp_results:
                st.error("Failed to get results from C++ engine!")
            else:
                # Display results
                st.success("✅ Analysis Complete!")
                st.markdown("---")
                
                # Key Metrics
                st.header("📊 Key Metrics")
                col1, col2, col3, col4 = st.columns(4)
                
                with col1:
                    confidence = cpp_results["confidence_score"]
                    st.metric(
                        label="Trade Confidence Score",
                        value=f"{confidence:.2f}",
                        delta="High" if confidence >=70 else "Medium" if confidence>=50 else "Low"
                    )
                
                with col2:
                    strategy = cpp_results["strategy"]
                    st.metric(
                        label="Recommended Strategy",
                        value=strategy
                    )
                
                with col3:
                    volatility = cpp_results["analytics"]["volatility"]
                    st.metric(
                        label="Volatility",
                        value=f"{volatility:.6f}"
                    )
                
                with col4:
                    sharpe = cpp_results["analytics"]["sharpe_ratio"]
                    st.metric(
                        label="Sharpe Ratio",
                        value=f"{sharpe:.4f}"
                    )
                
                col5, col6 = st.columns(2)
                with col5:
                    max_dd = cpp_results["analytics"]["max_drawdown"]
                    st.metric(
                        label="Max Drawdown",
                        value=f"{max_dd:.4%}"
                    )
                
                with col6:
                    st.metric(
                        label="Sentiment",
                        value=sentiment["label"],
                        delta=f"{sentiment['score']:.4f}"
                    )
                
                # Price Chart
                st.markdown("---")
                st.header("📉 Price Chart")
                fig = go.Figure(data=[
                    go.Candlestick(
                        x=df.index,
                        open=df["Open"],
                        high=df["High"],
                        low=df["Low"],
                        close=df["Close"],
                        name="OHLC"
                    )
                ])
                fig.update_layout(
                    xaxis_title="Date",
                    yaxis_title="Price",
                    xaxis_rangeslider_visible=False,
                    height=600
                )
                st.plotly_chart(fig, use_container_width=True)
                
                # RSI Chart
                st.header("📊 Technical Indicators")
                col_rsi, col_macd = st.columns(2)
                
                with col_rsi:
                    rsi = cpp_results["analytics"]["rsi"]
                    if len(rsi) > 0:
                        rsi_df = pd.DataFrame({
                            "Date": df.index[-len(rsi):],
                            "RSI": rsi
                        }).set_index("Date")
                        fig_rsi = go.Figure()
                        fig_rsi.add_trace(go.Scatter(
                            x=rsi_df.index,
                            y=rsi_df["RSI"],
                            mode="lines",
                            name="RSI",
                            line=dict(color="blue")
                        ))
                        fig_rsi.add_hline(y=70, line_dash="dash", line_color="red", name="Overbought")
                        fig_rsi.add_hline(y=30, line_dash="dash", line_color="green", name="Oversold")
                        fig_rsi.update_layout(
                            title="RSI (14)",
                            xaxis_title="Date",
                            yaxis_title="RSI",
                            yaxis_range=[0, 100],
                            height=400
                        )
                        st.plotly_chart(fig_rsi, use_container_width=True)
                    else:
                        st.info("Not enough data to compute RSI.")
                
                # MACD Chart
                with col_macd:
                    macd_line = cpp_results["analytics"]["macd_line"]
                    signal_line = cpp_results["analytics"]["signal_line"]
                    histogram = cpp_results["analytics"]["histogram"]
                    if len(macd_line) > 0:
                        macd_df = pd.DataFrame({
                            "Date": df.index[-len(macd_line):],
                            "MACD Line": macd_line,
                            "Signal Line": signal_line[-len(macd_line):],
                            "Histogram": histogram[-len(macd_line):]
                        }).set_index("Date")
                        fig_macd = go.Figure()
                        fig_macd.add_trace(go.Scatter(
                            x=macd_df.index,
                            y=macd_df["MACD Line"],
                            mode="lines",
                            name="MACD Line",
                            line=dict(color="blue")
                        ))
                        fig_macd.add_trace(go.Scatter(
                            x=macd_df.index,
                            y=macd_df["Signal Line"],
                            mode="lines",
                            name="Signal Line",
                            line=dict(color="orange")
                        ))
                        fig_macd.add_trace(go.Bar(
                            x=macd_df.index,
                            y=macd_df["Histogram"],
                            name="Histogram",
                            marker_color="green"
                        ))
                        fig_macd.update_layout(
                            title="MACD",
                            xaxis_title="Date",
                            yaxis_title="Value",
                            height=400
                        )
                        st.plotly_chart(fig_macd, use_container_width=True)
                    else:
                        st.info("Not enough data to compute MACD.")
    except Exception as e:
        st.error(f"An error occurred: {str(e)}")
        import traceback
        st.error(f"Details:\n{traceback.format_exc()}")
