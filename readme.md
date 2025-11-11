# 🧠 Process Manager

**Process Manager** is a lightweight, interactive, terminal-based system monitor written in modern **C++** for **Linux**.  
It allows you to view, sort, and manage your running processes directly from the terminal — similar to tools like **top** or **htop**, but with a clean and responsive interface built using **FTXUI**.

---

## 🎥 Preview

https://github.com/user-attachments/assets/b7cf7133-7eac-411b-8b05-f9d9e48f280d

---

## ⚙️ Features

- 🧾 View detailed information about all active processes
- 🔍 Sort processes by **CPU usage** or **memory usage**
- ⚔️ Kill unresponsive or unwanted processes directly from the UI
- ⏱️ Real-time updates for CPU and memory statistics
- 🧭 Navigate between processes interactively
- 🪶 Lightweight and responsive — no unnecessary overhead

---

## 🎮 Keybindings

| Key   | Action                                    |
| ----- | ----------------------------------------- |
| `↑/↓` | Navigate through process list             |
| `i`   | Toggle detailed information panel         |
| `s`   | Open sort options (CPU/Memory)            |
| `k`   | Kill selected process (with confirmation) |
| `q`   | Quit application                          |

---

## 🚀 Installation & Usage

### Building from Source

```bash
# Clone the repository
git clone https://github.com/YahiaJouini/process-manager.git ~/.local/bin/process-manager
cd ~/.local/bin/process-manager

# Build the project
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run it
./procmanager
```

### Quick Access with Alias

Add to your `~/.bashrc` or `~/.zshrc`:

```bash
alias procm="$HOME/.local/bin/process-manager/build/procmanager"
```

Reload your shell configuration:

```bash
source ~/.bashrc  # or source ~/.zshrc
```

Now you can run it from anywhere:

```bash
procm
```

---

## 📋 Requirements

- **C++17** or higher
- **CMake 3.10+**
- **FTXUI** library
- **Linux/Unix** system (uses `/proc` filesystem)

---

**Built with C++ and FTXUI** 🛠️
