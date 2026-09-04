# Sorta — High-Speed Bulk File Renamer & Organizer

[![Build and Package](https://github.com/1abdullahr1/sorta/actions/workflows/build-and-release.yml/badge.svg)](https://github.com/1abdullahr1/sorta/actions/workflows/build-and-release.yml)
[![Platform](https://img.shields.io/badge/platform-Windows%20x64-blue.svg)](https://github.com/1abdullahr1/sorta)
[![Language](https://img.shields.io/badge/C%2B%2B-17-00599C.svg?logo=c%2B%2B)](https://isocpp.org/)
[![Framework](https://img.shields.io/badge/Qt-6-41CD52.svg?logo=qt)](https://www.qt.io/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

**Sorta** is a modern, blazing-fast native Windows desktop utility designed to organize and rename thousands of files in milliseconds. Built with **C++17** and **Qt 6**, Sorta gives you full control over your files with zero bloat, instant real-time previews, safe conflict prevention, and 1-click undo.

---

## Features

- ⚡ **High-Speed Non-Blocking Scanner**: Traverses folders with 100,000+ files asynchronously in the background. The user interface never freezes.
- 👁️ **Instant Live Preview**: See exact before-and-after names (`Current Name → New Name`) in real-time as you type.
- 🛠️ **Powerful Renaming Engine**:
  - **Prefix & Suffix**: Prepend or append text easily.
  - **Find & Replace**: Case-sensitive text search and full Regular Expression (Regex) support.
  - **Sequential Numbering**: Prefix, suffix, or replace with custom starting numbers, step increments, and zero-padding (`001`, `0001`).
  - **Text Removal**: Strip out specific characters or words.
  - **Capitalization**: Convert names to lowercase, UPPERCASE, Title Case, Sentence case, or camelCase.
  - **Timestamps**: Insert modified or current dates with customizable date formats (`yyyy-MM-dd`).
  - **Extension Changer**: Modify or normalize file extensions.
  - **Template Pattern Mode**: Flexible pattern syntax such as `{name}_{num:3}_{date}` or `{parent}_{num:2}`.
- 🗂️ **Bulk Rule-Based Organizer**:
  - Automatically sort files into categorized folders (**Pictures**, **Documents**, **Videos**, **Audio**, **Archives**, **Code**, **Programs**).
  - Organize by creation/modified date (`YYYY-MM`, `YYYY`, `YYYY/MM`).
  - Organize by file size thresholds.
  - Custom rules: Match on extensions or filename patterns to route files to designated target folders.
- 🛡️ **Filesystem Safety & Conflict Detection**:
  - Automatically detects duplicate target names within a batch before executing.
  - Detects pre-existing destination files on disk.
  - Checks against invalid Windows filename characters (`\ / : * ? " < > |`) and control characters.
  - Blocks reserved Windows device names (`CON`, `PRN`, `AUX`, `NUL`, `COM1-9`, `LPT1-9`).
- ⏪ **1-Click Undo & Transaction History**:
  - Every batch operation is recorded in a persistent journal.
  - Safely revert renames or moved files back to their original names and locations anytime.
- 🎨 **Modern Windows Experience**:
  - Modern Windows 11 / Material 3-inspired design.
  - Drag-and-drop support: drag folders or files directly into Sorta.
  - Fast search and filter for large file lists.
  - High-DPI display scaling support.

---

## Download & Installation

### Option 1: Official Windows Installer (Recommended)
Download `Sorta_Setup_v1.0.0.exe` from the latest [GitHub Release](https://github.com/1abdullahr1/sorta/releases) or the Actions Artifacts.
1. Run `Sorta_Setup_v1.0.0.exe`.
2. Follow the setup wizard to install Sorta with Start Menu and Desktop shortcuts.
3. Clean uninstaller is registered in Windows Settings / Control Panel.

### Option 2: Portable Standalone ZIP
Download `Sorta_v1.0.0_Portable.zip`.
1. Extract the ZIP archive anywhere on your PC.
2. Run `Sorta.exe` directly — all Qt runtime DLLs and plugins are pre-packaged!

---

## Building from Source

### Prerequisites
- Windows 10/11 (x64)
- CMake 3.16+
- Ninja or MSVC 2022
- Qt 6 (6.2+ recommended) with `qtbase` (Core, Gui, Widgets)
- Inno Setup 6 (optional, for compiling the installer)

### Build Commands
```powershell
# Clone repository
git clone https://github.com/1abdullahr1/sorta.git
cd sorta

# Configure with CMake
cmake -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.7.2/msvc2022_64"

# Build executable
cmake --build build --config Release

# Deploy Qt dependencies into a standalone dist folder
mkdir dist
copy build\Sorta.exe dist\Sorta.exe
windeployqt --release --compiler-runtime --no-translations dist\Sorta.exe

# Build Installer (Requires Inno Setup)
& "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" /DSourceDir="$PWD\dist" installer\sorta_installer.iss
```

---

## Project Structure

```
sorta/
├── CMakeLists.txt                    # Top-level CMake build file
├── LICENSE                            # MIT License
├── README.md                          # Project overview & documentation
├── .gitignore                         # Git exclusion rules
├── .github/
│   └── workflows/
│       └── build-and-release.yml      # CI/CD: Automated build, windeployqt, Inno Setup & Release
├── installer/
│   └── sorta_installer.iss            # Inno Setup installer script
├── resources/
│   ├── app.ico                        # Multi-resolution application icon
│   ├── app.png                        # Application branding icon
│   ├── app.rc                         # Windows PE version and icon resource
│   ├── resources.qrc                  # Qt Resource bundle
│   └── styles/
│       └── modern.qss                 # Windows 11/Material stylesheet
└── src/
    ├── main.cpp                       # App entry point, DPI scaling, theme setup
    ├── core/
    │   ├── FileItem.h                 # File metadata and preview structure
    │   ├── FileScanner.h/.cpp         # Multi-threaded background scanner
    │   ├── RenameEngine.h/.cpp        # Rules, tokens, regex, numbering logic
    │   ├── OrganizeEngine.h/.cpp      # File organizer and rule categorization
    │   ├── ConflictDetector.h/.cpp    # Win32 character validation and collision check
    │   └── OperationHistory.h/.cpp    # Undo journal and transaction persistence
    └── ui/
        ├── MainWindow.h/.cpp          # Main window, drag & drop, coordinator
        ├── FileTableModel.h/.cpp      # Virtualized high-speed table model
        ├── RenamePanel.h/.cpp         # Live renaming controls
        ├── OrganizePanel.h/.cpp       # Organization controls and custom rules
        ├── HistoryDialog.h/.cpp       # Transaction history and revert dialog
        └── AboutDialog.h/.cpp         # About and credits dialog
```

---

## License

Sorta is open-source software licensed under the [MIT License](LICENSE).
Copyright (c) 2026 Abdullah Bhatti.
