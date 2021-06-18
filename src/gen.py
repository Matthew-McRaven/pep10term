import os 
from pathlib import Path
import subprocess

"""
Helper utility to convert text file into a XXD'ed include file.
"""

if __name__ == "__main__":
	path = Path(__file__)
	subprocess.run(["xxd", "-i", f"about.txt", f"gen_about.h"], cwd=path.parent)