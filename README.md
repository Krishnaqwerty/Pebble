# 🌊 **Pebble Programming Language**

> A tiny, beginner‑friendly language with a minimal syntax and a portable VM.
>
> **Author:** Krishna Kumar · **Email:** [meet.kumarkrishna@gmail.com](mailto:meet.kumarkrishna@gmail.com) · **GitHub:** [Krishnaqwerty](https://github.com/Krishnaqwerty)

---

## 🚀 Quick Install (macOS/Homebrew)

```bash
brew tap Krishnaqwerty/pebble
brew install pebble
```

Verify:

```bash
pebble --version
```

Expected:

```
Pebble language version 1.0.0
```

> 💡 If Homebrew isn’t available, see “Manual Install” below.

---

## 📦 Manual Install (from released binary or source)

```bash
# Clone and build
git clone https://github.com/Krishnaqwerty/Pebble.git
cd Pebble
make
```

---

## 🧪 Hello World in Pebble

Create a file named `hello.peb` with the following code:

```
say "Hello, Pebble!"
```

Save the file, then run it:

```bash
pebble hello.peb
```

You should see:

```
Hello, Pebble!
```

---

## 🖥️ CLI Usage

```bash
pebble <file.peb>
pebble --version
```

* `--version` prints the CLI/language version
* Exits non‑zero on file open/parse/runtime errors

---

## 👤 Developer

**Krishna Kumar**
📧 **Email:** [meet.kumarkrishna@gmail.com](mailto:meet.kumarkrishna@gmail.com)
💻 **GitHub:** [Krishnaqwerty](https://github.com/Krishnaqwerty)

> 🌈 Built to be small, clear, and extendable — so you can learn the fundamentals and then push the boundaries.
