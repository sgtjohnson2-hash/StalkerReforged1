# Stalker Reforged: The GAMMA Overhaul

This repository contains a comprehensive, ground-up remaster of classic S.T.A.L.K.E.R. mechanics built natively for the **Enfusion Engine**, transforming Arma Reforger into a hardcore Zone survival experience. 

Utilizing highly optimized, component-based Enforce Script (`.c`) instead of legacy procedural SQF loops or Lua, this project brings the brutal progression gameplay of **S.T.A.L.K.E.R. GAMMA** alongside fully procedural anomalies, missions, and military threat escalation.

---

## Extensive Core Features

### 🩸 Core Survival & Medical Engineering

- **Overhauled Medical (`SCR_GammaMedicalItemComponent`)**: Replaces instant-heals with complex Heal-over-Time arrays. Granular tracking for Medkits, Bandages hooking directly into Arma's bleeding system, and a perpetual `SCR_RadiationSicknessComponent` that scales ambient mSv damage over time.
- **Physical Needs (`SCR_SurvivalManagerComponent`)**: Slowly drains Thirst, Hunger, and Sleep. Neglecting your physiological stats results in brutal, escalating passive damage penalties.
- **Weapon Degradation (`SCR_WeaponDurabilityComponent`)**: Guns degrade incrementally directly correlating with every fired shot. Reaching critical condition thresholds mathematically subjects you to stochastic firing-pin jams.

### 🛠️ The Workbench Loop

- **Interactive spatial crafting (`SCR_WorkbenchComponent`)**: Interactable Workbench prefabs that bridge from the 3D world directly into structured Enfusion `.layout` graphic interfaces. 
- **Scrap Economy (`SCR_RepairSystemComponent`)**: Eliminate the need to magically snap guns to 100% durability. Combine scavenged inventory scrap to algorithmically restore weapon conditions.

### 📟 S.T.A.L.K.E.R. PDA Network

- **Dynamic Task Architecture (`SCR_PDATaskManagerComponent`)**: Built natively off Arma Reforger's underlying `SCR_BaseTask` system. Procedural Stalker tasks are automatically piped directly to the player's vanilla HUD and compass.
- **Ambient & Active Chat (`SCR_PDANetworkManager`)**: Monitors raw server death events (`OnDestroyed`) and transmits real Faction/Player assassinations across the PDA chatfeed, woven together with ambient spoofed STALKER lore and Emission warnings. Features audio hooks mimicking the physical Stalker PDA draw and beep noises.

### 🗺️ The Dynamic Map Systems
The world of Stalker Reforged isn't static. It generates its own gameplay loops dynamically on the server:

- **Procedural Loot Stashes (`SCR_ProceduralStashManager`)**: Removes the reliance on map traders. `SCR_LootableCorpseComponent` guarantees a mathematical % chance that killing an AI drops "coordinates". The server then geometrically spawns a physical Ammo Box wilderness stash dynamically anywhere from 500m to 3000m away, forcing you to seek it out. Stashes pull dynamically from Basic, Advanced, or Rare item pools (`SCR_StashLootTable`).
- **Bounty & Faction Contracts (`SCR_GammaMissionManager`)**: The PDA generates aggressive radial missions ("Wipe out FIA Patrol"). The engine mathematically picks a zone near you and dynamically utilizes `GetGame().SpawnEntityPrefab` to manifest actual physical FIA kill targets on the map boundary, natively tracking their deaths via custom entity components to grant RU rewards.
- **Helicopter Gunship QRFs & Strongholds (`SCR_MilitaryQRFManager`)**: You are not alone in the Zone. `SCR_ZoneHeatManager` silently tracks every bullet and body dropping across the server. Crack the heat threshold, and the Military acts. The engine will procedural spawn a **Soviet Mi-8 Helicopter** in the sky, inject a Spetsnaz troop directly into its bays, and provide an aggressive `SearchAndDestroy` waypoint over the hotzone coordinates.
- **Static Base Anchors (`SCR_StrongholdComponent`)**: A drag-and-drop component permitting server owners to establish pre-populated Soviet military garrisons that auto-generate defensive patrols immediately on server start.

### ⚡ Classic Anomalies & Blowouts

- **Component Anomalies**: Base framework allowing custom damage, visuals, and logic for Burner, Electra, and Teleport anomalies.
- **Dynamic Blowout Events**: A unified Game System (`SCR_BlowoutSystem`) tightly woven with Reforger's dynamic weather manager to darken skies, ramp up thick fog, and cast global storms. Includes an 'Instakill' exposure mechanic for players caught outside their `SCR_PlayerBlowoutHandlerComponent` roof-raycast bounds.

---

## Installation & Setup Instructions

Because this is a developer codebase (source mod folder) for Arma Reforger, you need to compile and set it up inside the **Enfusion Workbench**.

1. **Launch ARMA Reforger Workbench.**
2. On the welcome screen, click on **Create New Project**. 
   - Note: If you want to use the boilerplate project file provided, you can simply try to **Open Project** and select the `addon.gproj` file found in the root of this folder instead. 
3. **Copy the Source Code**:
   - If you created a new project in Workbench, close Workbench and copy the entire `Scripts/`, `UI/`, and `Prefabs/` folders from this directory into your newly generated Workbench Addon directory (usually located in `Documents/My Games/ArmaReforgerWorkbench/addons/YourName`).
4. **Re-open Workbench** and launch your project in the **World Editor**.
5. **Asset Import Restrictions**:
   - **Important Note on Models:** This repository provides the Enforce Scripts. Existing source S.T.A.L.K.E.R. models are encapsulated in a legacy `.ogf` format. Arma Reforger requires `.fbx` conversion and manual skeleton mapping to output `.xob` objects.
   - For all major mechanic testing, the codebase utilizes pre-existing vanilla Reforger `.et` paths (like FIA Riflemen for assassination bounties and Soviet Mi-8s for QRF Gunships).
6. **Playing/Testing**:
   - Make sure your local `Character_Base.et` player prefab has the necessary Manager scripts attached to it via the components tab.
   - Hit **Play** (F5) inside the World Editor. Open your PDA, generate tasks, establish strongholds, or kill enough AI to provoke a Gunship!
