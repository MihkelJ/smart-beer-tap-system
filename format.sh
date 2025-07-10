#!/bin/bash
# Code formatting script for Beer Tap project
#
# This script formats all C++ source files using clang-format
# Run from project root directory

echo "🔧 Formatting Beer Tap project code..."

# Format all C++ files
find . -name "*.cpp" -o -name "*.h" -o -name "*.ino" | xargs clang-format -i

echo "✅ Code formatting complete!"

# Verify compilation still works
echo "🧪 Verifying compilation..."
arduino-cli compile --fqbn esp32:esp32:esp32 .

if [ $? -eq 0 ]; then
    echo "✅ Compilation successful after formatting!"
else
    echo "❌ Compilation failed after formatting!"
    exit 1
fi