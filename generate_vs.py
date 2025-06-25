import subprocess
import sys
import os
import shutil

VS_VERSION = "vs2022"
BUILD_CONFIGURATIONS = ["Debug", "Release"]

try:
    result = subprocess.run(["tools/premake5", VS_VERSION], check=True)
    print("Visual Studio project files generated successfully.")
except subprocess.CalledProcessError as e:
    print("Failed to generate Visual Studio project files.")
    sys.exit(e.returncode)