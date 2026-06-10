import subprocess
import json
import os
from data_fetcher import fetch_ohlcv, save_data
from sentiment_engine import SentimentEngine

def run_cpp_engine(input_data: dict, exe_path: str) -> dict:
    """Call C++ drishti.exe with JSON input."""
    try:
        result = subprocess.run(
            [exe_path],
            input=json.dumps(input_data).encode("utf-8"),
            capture_output=True,
            text=False,
            check=True
        )
        return json.loads(result.stdout.decode("utf-8"))
    except subprocess.CalledProcessError as e:
        print(f"Error calling C++ engine: {e.stderr.decode('utf-8')}")
        return {}
    except Exception as e:
        print(f"Error: {str(e)}")
        return {}

if __name__ == "__main__":
    # Configuration
    TICKER = "AAPL"
    PERIOD = "1y"
    INTERVAL = "1d"
    SENTIMENT_HEADLINE = "Apple reports record quarterly revenue, beats earnings estimates"
    
    # Step 1: Fetch market data
    print(f"Fetching market data for {TICKER}...")
    market_data = fetch_ohlcv(TICKER, PERIOD, INTERVAL)
    save_data(market_data, f"{TICKER.lower()}_ohlcv.json")
    
    # Step 2: Analyze sentiment
    print("Analyzing sentiment...")
    sentiment_engine = SentimentEngine()
    sentiment_result = sentiment_engine.analyze(SENTIMENT_HEADLINE)
    print(f"Sentiment: {sentiment_result['label']} (score: {sentiment_result['score']:.4f})")
    
    # Step 3: Prepare input for C++ engine
    cpp_input = {
        "close": market_data["close"],
        "sentiment_score": sentiment_result["score"]
    }
    
    # Step 4: Run C++ engine
    print("Running C++ analytics and strategy engine...")
    project_root = os.path.dirname(os.path.dirname(__file__))
    cpp_exe = os.path.join(project_root, "cpp", "drishti.exe")
    cpp_output = run_cpp_engine(cpp_input, cpp_exe)
    
    if not cpp_output:
        print("Failed to get results from C++ engine!")
        exit(1)
    
    # Step 5: Save final results
    final_output = {
        "ticker": TICKER,
        "market_data": market_data,
        "sentiment": sentiment_result,
        "results": cpp_output
    }
    output_path = os.path.join(project_root, "output", f"{TICKER.lower()}_results.json")
    with open(output_path, "w") as f:
        json.dump(final_output, f, indent=4)
    print(f"Final results saved to: {output_path}")
    
    # Print key results
    print("\n=== Key Results ===")
    print(f"Trade Confidence Score: {cpp_output['confidence_score']:.2f}")
    print(f"Recommended Strategy: {cpp_output['strategy']}")
    print(f"Current Volatility: {cpp_output['analytics']['volatility']:.6f}")
    print(f"Sharpe Ratio: {cpp_output['analytics']['sharpe_ratio']:.4f}")
    print(f"Max Drawdown: {cpp_output['analytics']['max_drawdown']:.4%}")
