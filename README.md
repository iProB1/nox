# 🌙 NOX - Kernel Library

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Windows](https://img.shields.io/badge/Windows-10%2B%20%7C%2011-brightgreen.svg)](https://www.microsoft.com/windows)
[![Platform](https://img.shields.io/badge/Platform-x64-red.svg)]()

> **Stealth-focused kernel module resolver with zero PE imports**

---

## ✨ Features

- **🎯 Zero PE Imports** - Only `PsLoadedModuleList` is imported, everything else is dynamically resolved
- **🕵️ Stealth First** - Designed for minimal detection footprint
- **⚡ Lightweight** - Header-only, no external dependencies
- **🔧 Universal** - Works with any kernel module (ntoskrnl, win32k, CI.dll, etc.)
- **📦 No CRT Required** - Custom string functions, no runtime dependencies
- **🚀 High Performance** - Everything is `__forceinline`

---

## 📋 Requirements

- Windows 10 / 11 (x64)
- Visual Studio 2022+
- WDK (Windows Driver Kit)

---

## 🔧 Installation

Simply copy `nox.hpp` and `nt.hpp` to your project:

```bash
project/
├── nox.hpp    # Base class for any module
├── nt.hpp     # ntoskrnl specialization
└── driver.cpp # Your code
