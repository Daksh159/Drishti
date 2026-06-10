import warnings
warnings.filterwarnings("ignore")
import os
os.environ["TRANSFORMERS_NO_ADVISORY_WARNINGS"] = "1"
os.environ["TRANSFORMERS_VERBOSITY"] = "error"
os.environ["TF_CPP_MIN_LOG_LEVEL"] = "3"
import logging
logging.getLogger("transformers").setLevel(logging.ERROR)
logging.getLogger("torch").setLevel(logging.ERROR)
import sys
from io import StringIO
# Redirect stdout to suppress transformers' print warnings
old_stdout = sys.stdout
sys.stdout = StringIO()
# Now import transformers
from transformers import AutoTokenizer, AutoModelForSequenceClassification
import torch
import json
# Restore stdout
sys.stdout = old_stdout


class SentimentEngine:
    def __init__(self, model_name: str = "ProsusAI/finbert"):
        self.tokenizer = AutoTokenizer.from_pretrained(model_name)
        self.model = AutoModelForSequenceClassification.from_pretrained(model_name)
        self.model.eval()
        
    def analyze(self, text: str) -> dict:
        """
        Analyze sentiment of financial text.
        
        Returns:
            Dict with 'label' (bullish/neutral/bearish) and 'score' (-1 to +1)
        """
        inputs = self.tokenizer(text, return_tensors="pt", truncation=True, padding=True)
        with torch.no_grad():
            outputs = self.model(**inputs)
        
        probabilities = torch.nn.functional.softmax(outputs.logits, dim=-1).numpy()[0]
        # ProsusAI/finbert labels: 0=positive, 1=negative, 2=neutral
        max_idx = probabilities.argmax()
        
        label_map = {0: "bullish", 1: "bearish", 2: "neutral"}
        label = label_map[max_idx]
        
        # Convert to -1 to +1 score
        if label == "bullish":
            score = probabilities[0]
        elif label == "bearish":
            score = -probabilities[1]
        else:
            score = 0.0
            
        return {
            "text": text,
            "label": label,
            "score": float(score)
        }


if __name__ == "__main__":
    # Test with sample headlines
    engine = SentimentEngine()
    
    sample_headlines = [
        "Apple reports record quarterly revenue, beats earnings estimates",
        "Reliance Industries announces major expansion into renewable energy",
        "NIFTY 50 drops 2% due to global market sell-off",
        "Inflation data comes in higher than expected, causing market uncertainty",
        "Tech stocks rally as AI investments drive future growth expectations"
    ]
    
    results = []
    print("Analyzing sample headlines...\n")
    for i, headline in enumerate(sample_headlines, 1):
        result = engine.analyze(headline)
        results.append(result)
        print(f"Headline {i}: {result['text']}")
        print(f"  Sentiment: {result['label']}")
        print(f"  Score: {result['score']:.4f}\n")
    
    # Save results
    output_dir = os.path.join(os.path.dirname(os.path.dirname(__file__)), "output")
    os.makedirs(output_dir, exist_ok=True)
    filepath = os.path.join(output_dir, "sentiment_results.json")
    with open(filepath, "w") as f:
        json.dump(results, f, indent=4)
    print(f"Results saved to {filepath}")
