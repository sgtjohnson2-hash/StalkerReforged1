# S.T.A.L.K.E.R. Anomalies & Emissions Remaster for Arma Reforger

This is a comprehensive remaster and combination of the classic Arma 3 modifications (`stalker_anomalies` and `a3_emission`). It has been completely rebuilt from the ground up for the **Enfusion Engine**, utilizing heavily optimized component-based Enforce Script architecture (`.c`) instead of older procedural SQF loops. 

## Features
- **Component Anomalies**: Base framework allowing custom damage, visuals, and logic for Burner, Electra, and Teleport anomalies.
- **Blowout Server Event**: A unified Game System (`SCR_BlowoutSystem`) that integrates tightly with Reforger's dynamic weather manager to darken skies, ramp up fog, and cast terrifying global storms.
- **Instakill Blowouts**: Players caught outside during a blowout without roof cover (`SCR_PlayerBlowoutHandlerComponent`) will suffer fatal consequences.
- **Equipment Framework**: Base detector logic for audio/visual pinging of anomalies, alongside throwable bolts which trigger anomaly visual effects without consuming charges.
- **Procedural and Manual Placement**: `SCR_AnomalySpawnerComponent` allows map-wide radial auto-population, but all anomalies are drag-and-drop compatible prefabs for manual Level Design.

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
   - The `/Sounds` folder contains raw `.ogg` audio files. You will need to right click them in the Resource Browser and import them into Reforger's `.acp` audio format.
   - The `/UI` folder contains `.png/.paa` files. These should be imported mechanically via Workbench as Texture files (`.edds`).
6. **Prefab Setup**:
   - In the Workbench Resource Browser, right-click and create a new **Prefab** extending `GenericEntity` (e.g. `BurnerAnomaly.et`).
   - Add the `SCR_BurnerAnomalyComponent` to the Prefab.
   - You can now drag and drop these prefabs into your map!
7. **Playing/Testing**:
   - Hit **Play** (F5) inside the World Editor to launch the instance and verify that entering your placed anomalies triggers print logs and takes your player's script-health!

## Roadmap for Developers
- Assign the imported Particle Effects to the respective strings inside the Anomaly Components.
- Assign your imported audio clips (`.acp`) to be triggered by the Detector script and the Blowout siren callbacks.
