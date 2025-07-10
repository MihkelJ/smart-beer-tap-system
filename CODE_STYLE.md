# Code Style Guidelines

## Automated Formatting

This project uses **clang-format** for consistent code formatting. All C++ source files (`.cpp`, `.h`, `.ino`) are automatically formatted according to the style configuration in `.clang-format`.

### Usage

#### Format all files:
```bash
./format.sh
```

#### Format specific files:
```bash
clang-format -i src/pour_system.cpp
clang-format -i *.ino
```

#### Check formatting without changes:
```bash
clang-format --dry-run --Werror *.cpp *.h *.ino
```

## Style Configuration

The project uses a **Google-based** style with the following key adjustments for Arduino/embedded development:

- **Indentation**: 2 spaces (no tabs)
- **Line length**: 100 characters
- **Braces**: Attached style (`{` on same line)
- **Spacing**: Standard spacing around operators and keywords
- **Pointers**: Left-aligned (`int* ptr`)
- **Comments**: Reflowed and properly aligned

### Example Formatting:

```cpp
// Good formatting
void PourSystem::startPour() {
  isPouring = true;
  setRelay(false);
  pourStartTime = millis();
  Serial.println("Pour started");
}

// Function declarations
bool performSafetyChecks(bool wifiConnected, bool thingsBoardConnected);

// Constants and macros
#define MAX_POUR_TIME 90000  // Maximum pour time in milliseconds
```

## Pre-commit Formatting

Consider setting up pre-commit hooks to automatically format code before commits:

```bash
# Add to .git/hooks/pre-commit
#!/bin/bash
./format.sh
git add .
```

## IDE Integration

### Visual Studio Code:
1. Install **C/C++** extension
2. Install **Clang-Format** extension  
3. Set format on save in settings.json:
```json
{
    "editor.formatOnSave": true,
    "C_Cpp.clang_format_style": "file"
}
```

### Arduino IDE:
The Arduino IDE doesn't have built-in clang-format support, but you can format files externally and reload them in the IDE.

## Benefits

- **Consistency**: All code follows the same style
- **Readability**: Clean, properly indented code
- **Collaboration**: Eliminates style-related discussions
- **Maintenance**: Easier to review and maintain code
- **Professional**: Industry-standard formatting practices