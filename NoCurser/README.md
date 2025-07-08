# Vim-NoCursor: A Minimalistic Cursorless Vim Fork

## Overview

Vim-NoCursor is a minimalist fork of Vim that removes the terminal cursor and simplifies the interface for a distraction-free editing experience. This modification is particularly useful for:

- Screencasts and presentations
- Minimalist development environments
- Users who prefer keyboard-only navigation

## Key Features

- **No visible cursor**: The terminal cursor is permanently hidden
- **Simplified interface**: Removes status bars, mode indicators and other visual clutter
- **Lightweight**: Reduced feature set for faster operation
- **Custom branding**: Clearly identifies as Vim-NoCursor

## Installation

1. Clone the Vim-NoCursor repository:
   ```bash
   git clone https://github.com/braga-waysward/Vim-NoCurser
   cd Vim-NoCurser/NoCurser
   ```

2. Compile and run the setup tool:
   ```bash
   g++ setup.cpp -o setup -std=c++17
   ./setup
   ```

3. Build as normal:
   ```bash
   make
   sudo make install
   ```

## Usage

Run the modified Vim with:
```bash
nocursor [filename]
```

The editor will behave like standard Vim but without a visible cursor or most interface elements.

## Customisation

The setup tool automatically applies these modifications:

1. Configures the binary name as `nocursor`
2. Updates version information with "Vim-NoCursor" branding
3. Replaces the splash screen
4. Implements cursor-hiding functionality
5. Disables various UI elements by default

## License

As a derivative work, Vim-NoCursor inherits Vim's original license (Vim License).

## Notes

The setup process will:
- Display progress messages as it modifies each source file
- Indicate when modifications are complete
- Preserve the original functionality where not explicitly changed

Simply follow the on-screen instructions after running the setup tool.

---

Enjoy your distraction-free editing experience with Vim-NoCursor!
