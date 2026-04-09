# S.T.A.L.K.E.R. GAMMA & Anomalies Remaster for Arma Reforger

This repository contains a comprehensive ground-up remaster of classic S.T.A.L.K.E.R. mechanics built natively for the **Enfusion Engine**, transforming Arma Reforger into a hardcore Zone survival experience. Utilizing highly optimized component-based Enforce Script (`.c`) instead of legacy SQF loops or Lua, this project brings the brutal progression loops of **S.T.A.L.K.E.R. GAMMA** alongside fully procedural anomalies and emissions.

## Core Features

### 1. Advanced Anomalies & Emissions
- **Component Anomalies**: Base framework allowing custom damage, visuals, and logic for Burner, Electra, and Teleport anomalies. Drag-and-drop compatible prefabs for Level Design.
- **Dynamic Blowout Events**: A unified Game System (`SCR_BlowoutSystem`) integrated tightly with Reforger's dynamic weather manager to darken skies, ramp up fog, and cast global storms. Includes an 'Instakill' exposure mechanic for players caught outside their `SCR_PlayerBlowoutHandlerComponent` roof-raycast bounds.
- **Detector Framework**: Base detector logic for audio/visual pinging of anomalies, alongside throwable bolts triggering entity visual effects.

### 2. S.T.A.L.K.E.R. GAMMA Overhaul Module
A full suite of custom Enfusion scripts designed to enforce brutal survival, crafting, and task delegation directly inspired by Grokitach's GAMMA modpack:

- **Medical & Radiation Engine**: Replaces instant-heals with Heal-over-Time arrays (`SCR_GammaMedicalItemComponent`). Granular tracking for Medkits, Bandages (bleeding hooks), and a perpetual `SCR_RadiationSicknessComponent` that scales ambient mSv damage over time.
- **Needs & Degradation**: 
  - `SCR_SurvivalManagerComponent` drains Thirst, Hunger, and Sleep, resulting in health penalties if neglected.
  - `SCR_WeaponDurabilityComponent` causes weapons to degrade incrementally with every fired shot, leading to stochastic jams at critical thresholds.
- **Workbench & Repair Loop**: Introduces interactable Workbench prefabs (`SCR_WorkbenchComponent`) that bridge directly to an Enfusion `.layout` graphic interface. Players combine scavenged scrap and weapon kits to restore their `WeaponDurabilityComponent` properties.
- **Dynamic PDA Network**: 
  - Native integration with Arma Reforger's `SCR_BaseTask` and `SCR_BaseTaskManager`. Procedural Stalker tasks ("Fetch", "Assassination", "Hunt") are piped directly to the player's vanilla compass and HUD!
  - `SCR_PDANetworkManager` monitors your server events and transmits real AI/player assassinations across the PDA chatfeed, alongside ambient spoofed STALKER network lore and Emission early-warnings.

---

## Installation & Setup Instructions

Because this is a developer codebase (source mod folder) for Arma Reforger, you need to compile and set it up inside the **Enfusion Workbench**.

1. **Launch ARMA Reforger Workbench.**
2. On the welcome screen, click on **Create New Project**. 
   - Note: If you want to use the boilerplate project file provided, you can simply try to **Open Project** and select the `addon.gproj` file found in the root of this folder instead. 
3. **Copy the Source Code**:
   - If you created a new project in Workbench, close Workbench and copy the entire `Scripts/`, `Sounds/`, `UI/`, and `Prefabs/` folders from this directory into your newly generated Workbench Addon directory (usually located in `Documents/My Games/ArmaReforgerWorkbench/addons/YourName`).
4. **Re-open Workbench** and launch your project in the **World Editor**.
5. **Asset Import**:
   - **Important Note on Models:** This repository provides the Enforce Scripts. Existing S.T.A.L.K.E.R. assets are in legacy `.ogf` formats and must be manually converted to `.fbx` and passed through the Workbench to create `.xob` models and `.edds` textures.
   - For now, map the custom scripts (e.g. `SCR_StalkerMedkitComponent`) directly onto vanilla Reforger prefabs (`IFAK.et` or `Bandage.et`).
6. **Playing/Testing**:
   - Hit **Play** (F5) inside the World Editor. You can check the UI Print strings and network logs to verify degradation, dynamic PDA tasks, and anomaly triggers working flawlessly in real-time.

## Roadmap for Developers
- Build custom `.layout` ENF GUI menus mapping specifically to the `SCR_WorkbenchUI` and `SCR_PDA_UI` methods.
- Import `.fbx` 3D meshes for distinct PDA screens, toolboxes, AI-2 Medkits, and specific Mutants.
- Expand `EStalkerTaskType` enum handlers to spawn AI groups dynamically when specific Hunt Tasks natively initialize.
