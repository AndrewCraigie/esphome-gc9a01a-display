# GC9A01A Display Component for ESPHome

Custom ESPHome component for GC9A01A round TFT displays.

## Development Setup

### Initial Setup

1. Create virtual environment: `python -m venv venv`
2. Install ESPHome: `pip install esphome` (after activating environment)

### Daily Development Workflow

**Windows/Git Bash Users:**
```bash
# Use the provided activation script (recommended)
source activate-env.sh

# Then run ESPHome commands
esphome dashboard .
```

**Alternative (manual activation):**
```bash
# Set encoding and activate environment manually
export PYTHONIOENCODING=utf-8
source venv/Scripts/activate

# Then run ESPHome commands
esphome dashboard .
```

### Common Commands

- **Start web dashboard:** `esphome dashboard .`
- **Compile configuration:** `esphome compile reminder-display.yaml`
- **Upload to device:** `esphome upload reminder-display.yaml`
- **View logs:** `esphome logs reminder-display.yaml`

### Project Structure

```
esphome-gc9a01a-display/
├── components/
│   └── gc9a01a_display/        # Custom component files
├── shared/
│   ├── network.yaml            # Network configuration
│   └── secrets.yaml            # Shared secrets reference
├── reminder-display.yaml       # Main device configuration
├── secrets.yaml                # Actual secret values (not in git)
├── activate-env.sh             # Environment activation script
└── venv/                       # Python virtual environment
```

### Notes

- The `activate-env.sh` script sets `PYTHONIOENCODING=utf-8` to fix Windows logging encoding issues
- Secrets are managed in a two-tier structure (root secrets.yaml + shared reference)
- Use the ESPHome dashboard for a visual interface similar to Home Assistant's ESPHome builder