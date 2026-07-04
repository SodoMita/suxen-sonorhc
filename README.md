# Dialogic 2 Godot 4 Visual Novel Demo

This is a clean, minimal, and fully-functional starter template for a **Visual Novel (VN)** project in **Godot 4.x** using **Dialogic 2** (the highly anticipated, modern rewrite of the popular branching dialogue plugin).

## Project Structure

*   `addons/dialogic/` - The Dialogic 2 plugin files (Version `2.0-alpha-19` for Godot 4.3+).
*   `sora.dch` - Dialogic Character definition file (JSON format with custom extensions) representing our speaker **Sora**.
*   `main_timeline.dtl` - A text-based Dialogic timeline scripting custom events, branching choice paths, background switches, and animations.
*   `main.tscn` & `main.gd` - The main scene and controller script that boots up and hands over scene and input control to Dialogic via `Dialogic.start()`.
*   `speaker_neutral.png` & `speaker_happy.png` - Beautiful character portrait variants generated for Sora.
*   `vn_background.jpg` - A scenic anime school classroom background at sunset.
*   `project.godot` - Godot configuration file pre-enabling the Dialogic plugin and mapping the `dialogic_default_action` input triggers (Space, Enter, Left Mouse Click).

## Features

1.  **Modular Character Portraits:** Uses standard `.dch` character definitions. Swapping from `neutral` to `happy` is handled instantly inline in the script (`sora (happy): ...`).
2.  **Rich Branching Decisions:** Implements visual branch options using Dialogic 2's indentation syntax.
3.  **Automatic Input Handling:** Presets the necessary Input Map configurations directly so that players can immediately progress dialogues with Spacebar, Enter, or Mouse Left-Clicks.
4.  **Auto-repeating Timelines:** Demonstrates clean transitions, scene entry animations (`Bounce In`), scene exit animations (`Bounce Out`), background transitions, and loops seamlessly at the end of the narrative sequence.

## How to Run This Project

1.  Download or open this workspace in the **Godot Engine (Godot 4.3 or higher)**.
2.  Godot will automatically load and initialize the plugin located inside `addons/dialogic`.
3.  Click **Play** (or press `F5`) to watch the Visual Novel sequence start instantly!
