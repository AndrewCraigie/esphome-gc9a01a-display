#!/bin/bash

# ESPHome Development Environment Activation Script
# Sets UTF-8 encoding for Windows compatibility and activates Python virtual environment

echo "Setting up ESPHome development environment..."

# Set UTF-8 encoding to fix Windows logging issues
export PYTHONIOENCODING=utf-8
echo "✓ Set PYTHONIOENCODING=utf-8"

# Check if virtual environment exists
if [ ! -d "venv" ]; then
    echo "Virtual environment not found. Please run: python -m venv venv"
    exit 1
fi

# Activate virtual environment
source venv/Scripts/activate
echo "✓ Activated virtual environment"

echo "Environment ready! You can now run:"
echo "  esphome dashboard .        # Start web dashboard"
echo "  esphome compile <device>   # Compile configuration"
echo "  esphome upload <device>    # Upload to device"