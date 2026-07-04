# Chrono Nexus (suxen-sonorhc)

An immersive, high-fidelity visual novel built entirely inside **Godot 4.x** utilizing the powerful **Dialogic 2** dialogue engine. 

---

## 🌌 Lore & Narrative Concept

*Chrono Nexus* tells the story of **Ren**, an enigmatic foreign exchange student of undefined nationality attending high school in Tokyo. During a strange atmospheric shift in his school locker room, he is suddenly swept away into the **Chrono Nexus**—a beautiful, unstable dimension of floating crystals, dimensional rifts, and floating sacred temples.

In this dimension, Ren manifests not as a human, but as a shifting **shadow silhouette** with soft glowing cyan eyes. His shadow-energy serves as a biological stabilizer for the flickering dimension, drawing him close to the beautiful, powerful entities keeping the world together:

*   **Aurora** (Holographic Subsystem Construct): Vulnerable and deeply connected to Ren's core matrix.
*   **Kira** (Sunfire Catgirl): Playful, energetic, and highly reactive to Ren's mysterious presence.
*   **Elara** (Moon Elf Healer): Gentle, serene, wearing delicate silken vestments reactive to the rift wind.
*   **Selene** (Twilight Sorceress): Sophisticated, witty, and eager to study Ren's shadow composition under closer illumination.

The game explores five fundamental philosophical paradoxes (Identity, Connection, Home, Sacrifice, Memory) through charged physical proximity, costume shifting, and emotional breakthroughs.

---

## 📂 Project Structure

*   `addons/dialogic/` — The pre-integrated **Dialogic 2** engine.
*   `Sprites/` — A comprehensive set of high-fidelity character sprites with complete emotional kits (`neutral`, `happy`, `sad`, `surprised`, `blush`, `extreme_blush`).
    *   *Includes `player_shadow.png`, Ren's high-definition full-body shadow silhouette isolated on a solid white background for clean transparency mask removal.*
*   `bgs/` — Immersive backdrop scenery illustrating key locations throughout the campaign (Japanese Classroom, Nexus Chamber, Starry Sanctum, Grove, Beach Sunset, and the Reactor Core).
*   `player.dch`, `aurora.dch`, `kira.dch`, `elara.dch`, `selene.dch` — Mapped character files linking emotional expressions straight to their sprite profiles.
*   `TL_001_Opening_RiftSchool.dtl` — Act I: The classroom rift and meeting Aurora & Kira.
*   `TL_002_Sanctum_Arrival.dtl` — Act II: Floating star temples with Elara & Selene.
*   `TL_003_Whispers_In_The_Grove.dtl` — Act III: Intimate, quiet moments under the whisper trees.
*   `TL_004_Sanctum_Hub.dtl` — Act IV: Ancient memories over the Sunset Ocean.
*   `TL_005_Core_Resonance.dtl` — Act V: Absolute stabilization at the crystal core.
*   `main.tscn` & `main.gd` — Main scene bootloader starting the campaign dynamically.

---

## 🛠️ How to Play

1. Clone or download the repository to your local system.
2. Open the project folder in **Godot 4.3 or higher**.
3. Press **`F5`** (or click the Play button in the top-right corner) to watch the Chrono Nexus campaign start seamlessly from the Tokyo prologue!
4. Advance text using **Spacebar**, **Enter**, or **Left Mouse Click**.
