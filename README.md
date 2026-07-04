# Chrono Nexus (suxen-sonorhc)

An immersive, high-fidelity visual novel engine built entirely inside **Godot 4.x** utilizing the powerful **Dialogic 2** branching dialogue plugin.

---

## 🛠️ Project Setup & Usage

### 🚀 Getting Started

1. Clone or download this repository to your local machine.
2. Open the project folder in **Godot 4.3 or higher**.
3. Press **`F5`** (or click the Play button in the top-right corner of the editor) to run the game directly.

### 🎮 Default Controls
*   **Advance Dialogue / Select Choices**: `Spacebar`, `Enter`, or `Left Mouse Click`.
*   These mappings are configured automatically within the Godot project setting maps (`dialogic_default_action`).

---

## 📂 Project Architecture

*   `addons/dialogic/` — The complete Dialogic 2 plugin core files.
*   `characters/` — Mapped `.dch` character files and resource configs.
*   `timelines/` — Main story acts scripted in custom `.dtl` format.
*   `Sprites/` — Character graphic assets and expression suites.
*   `bgs/` — Environment and landscape backgrounds.
*   `docs/` — Expanded design documents, lore, and directories.
    *   [`docs/characters.md`](docs/characters.md) — Comprehensive database of all configured characters.
    *   [`docs/world.md`](docs/world.md) — Documentation on dimensional rules, environments, and physics.
*   `main.tscn` & `main.gd` — Primary scene node and boot loader logic.
