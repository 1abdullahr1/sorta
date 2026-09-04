# Build "Sorta" — High-Speed Bulk File Renamer & Organizer

I want to build a polished desktop application called **Sorta** — a high-speed bulk file renamer and organizer for Windows.

The core idea is simple: help users quickly clean up thousands of messy files without using complicated scripts or slow, bloated software.

## Technology

- **Rust** for the application backend and file-system operations.
- **Slint** for the desktop UI.
- Design the application as a proper native-feeling Windows desktop application.
- Prioritize performance, low memory usage, responsiveness, and reliability.
- The UI must **never freeze**, even while processing tens of thousands of files.

## Core Problem

Users often have folders containing thousands of files with useless names such as:

- `IMG_4837.jpg`
- `IMG_4838.jpg`
- `document (1).pdf`
- `document (2).pdf`
- `final_final_v3.docx`
- `download (123).zip`

Renaming and organizing these manually through Windows Explorer is tedious and error-prone.

Sorta should make bulk organization extremely fast while still being understandable to a normal non-technical user.

## Core Features

### 1. Fast Folder Scanner

Allow users to select a folder and quickly scan its contents.

Show:

- File name
- Extension
- Size
- Date modified
- File type
- Current location

Support large directories efficiently.

The UI should remain responsive during scanning.

### 2. Bulk Rename

Provide powerful but easy-to-understand rename operations.

Examples:

- Add prefix
- Add suffix
- Replace text
- Remove text
- Find & replace
- Sequential numbering
- Change capitalization
- Change file extension
- Add date
- Add original file metadata
- Custom naming patterns

Example:

`IMG_4837.jpg`

→ `Vacation_001.jpg`

→ `Vacation_002.jpg`

→ `Vacation_003.jpg`

### 3. Live Preview

Before applying changes, show users exactly what will happen.

Use a clear two-column preview:

**Current Name → New Name**

For example:

`IMG_4837.jpg → Vacation_001.jpg`

Users should be able to review the entire operation before committing it.

### 4. Safe Operations

Never blindly rename files.

Implement:

- Conflict detection
- Duplicate-name detection
- Invalid filename detection
- Permission error handling
- Locked-file detection
- Collision prevention
- Undo support where technically possible

Before executing a large operation, clearly tell the user what will happen.

### 5. Bulk File Organizer

Allow users to automatically move files into folders based on rules.

Examples:

- Images → `Pictures`
- Videos → `Videos`
- PDFs → `Documents/PDFs`
- ZIP files → `Archives`
- Files created in 2026 → `2026`
- Files larger than 1 GB → `Large Files`

Allow multiple rules and make the rule builder simple.

### 6. Rule-Based Organization

Users should be able to create rules such as:

`If extension = PDF → move to Documents/PDF`

or:

`If file name contains "invoice" → move to Documents/Invoices`

or:

`If file modified date is in 2026 → move to 2026`

### 7. Batch Processing

The application should be capable of processing:

- 100 files
- 1,000 files
- 10,000 files
- 100,000+ files

without unnecessary memory consumption.

Use Rust's concurrency and efficient file-system operations where appropriate, but prioritize **correctness and filesystem safety over blindly maximizing parallelism**.

### 8. Operation History

Show recent operations.

For example:

**Renamed 1,248 files**

`D:\Photos\2026`

Allow users to inspect what changed.

## UI/UX

The application should feel like a modern, professional Windows utility rather than a developer tool.

Design principles:

- Minimal
- Clean
- Fast
- Professional
- Spacious
- Easy to understand
- Strong visual hierarchy
- No unnecessary UI
- No gradients
- No excessive animations
- No emojis

Use **Material 3-inspired design principles**, but do not make the application look like a generic Android Material app. Give Sorta its own desktop identity.

The application should feel extremely responsive.

Animations should be subtle and purposeful.

## Main Screen

The main screen should immediately communicate the application's purpose.

Possible structure:

**Sorta**

`Organize your files faster.`

[ Select Folder ]

Then display the selected folder and its contents.

Provide clear primary actions such as:

- Rename
- Organize
- Preview
- Rules

Avoid cluttering the interface with advanced options until the user needs them.

## Performance Requirements

Performance is one of the most important parts of this project.

Use Rust efficiently for:

- Directory traversal
- File metadata collection
- Rename operations
- File moving
- Rule evaluation
- Batch processing

Do not load unnecessary file contents into memory.

Use asynchronous/background processing where necessary so that:

**The UI thread must never block on large filesystem operations.**

Display progress for long-running operations.

For example:

`Scanning 8,421 / 12,000 files`

and:

`Renaming 6,203 / 8,421`

## Architecture

Keep the project clean and maintainable.

Separate:

- Slint UI
- Application state
- File-system operations
- Rename engine
- Organization/rule engine
- Validation
- Operation history
- Error handling

Do not put all logic inside the UI layer.

Design the core file-processing engine so it could potentially be tested independently from the UI.

## Error Handling

Filesystem operations can fail.

Handle errors gracefully instead of crashing.

Examples:

- Access denied
- File already exists
- File is being used
- Invalid filename
- Drive disconnected
- File disappeared during scanning
- Insufficient permissions

Give users useful explanations instead of technical Rust errors.

## Windows Integration

Build this as a **proper installable Windows application**, not merely an `.exe` that the user manually runs.

Plan for:

- Windows installer
- Start Menu shortcut
- Desktop shortcut option
- Proper application icon
- Uninstaller
- Version information
- Application metadata

The final application should feel like software users can properly install and uninstall from Windows.

## Development Approach

Do not blindly implement everything at once.

First establish:

1. Project architecture
2. Slint UI foundation
3. Rust filesystem engine
4. Folder scanning
5. File table
6. Rename engine
7. Preview system
8. Safe execution
9. Organization rules
10. History/undo
11. Performance optimization
12. Windows packaging

At every stage, prioritize:

**Correctness → Safety → Responsiveness → Performance → Visual polish**

Do not sacrifice filesystem safety just to achieve benchmark numbers.

The final goal is to create a **small, extremely fast, polished, reliable Windows file-organization utility** that a normal user can understand within seconds.