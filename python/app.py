
# Suppress ALL output before importing anything else!
import warnings
warnings.filterwarnings("ignore")
import os
os.environ["TRANSFORMERS_NO_ADVISORY_WARNINGS"] = "1"
os.environ["TRANSFORMERS_VERBOSITY"] = "error"
os.environ["TF_CPP_MIN_LOG_LEVEL"] = "3"
import logging
logging.basicConfig(level=logging.ERROR)
logging.getLogger().setLevel(logging.ERROR)
logging.getLogger("transformers").setLevel(logging.ERROR)
logging.getLogger("torch").setLevel(logging.ERROR)
logging.getLogger("yfinance").setLevel(logging.ERROR)
import sys
from io import StringIO
# Completely redirect stdout/stderr to buffers for import phase
old_stdout = sys.stdout
old_stderr = sys.stderr
sys.stdout = StringIO()
sys.stderr = StringIO()

# Now import everything!
from dashboard import *

# Now restore stdout/stderr
sys.stdout = old_stdout
sys.stderr = old_stderr
