# 🖥️ MyOS - Custom Operating System

MyOS is a hobby operating system built from scratch using **C and Assembly**.  
It includes a basic kernel, framebuffer graphics, a simple shell, file system, and a custom desktop UI layer.

---

## 🚀 Features

- 🧠 Multiboot-compliant kernel (GRUB boot)
- ⌨️ Keyboard input handling (PS/2)
- 🖼️ Framebuffer-based graphics system
- 🪟 Basic desktop environment (custom UI layer)
- 📁 Simple in-memory file system
- 💻 Command-line shell (mkdir, rm, cd, ls, etc.)
- 🎨 Primitive drawing API (rectangles, lines, text rendering)

---

## 🖥️ Desktop System

MyOS includes a very early **desktop environment layer**:

- Background rendering
- Simple taskbar
- "Start" button UI element
- Basic drawing primitives (rect, text, fill)

⚠️ At this stage, the desktop is **purely visual** — you cannot interact with it yet.

---

## ⚙️ Build Instructions

### Requirements
- GCC (32-bit support)
- NASM
- GRUB
- QEMU (for testing)

### Build

```bash
make clean
make
