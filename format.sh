#!/bin/bash

echo "🎨 Formatting C/H files with clang-format..."

# Find and format all .c and .h files, excluding build and submodules
files=$(find . \( -name "*.c" -o -name "*.h" \) \
    -not -path "./build/*" \
    -not -path "./submodules/*")

count=$(echo "$files" | wc -l)
echo "📁 Found $count files to format"

if [ $count -eq 0 ]; then
    echo "❌ No files found to format"
    exit 1
fi

# Apply formatting
echo "$files" | xargs clang-format -i

echo "✅ Successfully formatted $count files!"
echo ""
echo "Files formatted:"
echo "$files" | sed 's|^\./||' | sort