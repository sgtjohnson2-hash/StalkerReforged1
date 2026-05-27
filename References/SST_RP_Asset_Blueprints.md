# S.T.A.L.K.E.R. RP Asset Blueprints & Workbench Naming Conventions

This document maps out the structured architecture for importing, configuring, and binding customized S.T.A.L.K.E.R. resources (Models, Weapons, Outfits, Sounds, Vehicles, and Animations) natively within the **Enfusion Workbench** for Arma Reforger, ensuring full compatibility with Stalker Reforged's scripting systems.

---

## 1. Directory Structure Blueprint
To maintain modularity and compatibility with the `addon.gproj` workspace, you must organize all imported assets under these standard folders:

```
YourModWorkspace/
├── addon.gproj               # Main project config
├── Configs/                  # Base game configs (Factions, Items)
│   ├── Factions/             # USSR, FIA, US, CIV modifications
│   └── LootTables/           # SCR_StashLootTable mapping configs
├── Prefabs/                  # Compiled game entities (.et)
│   ├── Characters/           # Mercenaries, Monolithians, Bandits, Loners
│   ├── Items/                # Medkits, Bandages, Food, Keycards, Seeds, Drugs
│   ├── Anomalies/            # Electra, Burner, Teleport prefabs
│   └── Vehicles/             # UAZ-469, BTR-70, Mutated transport platforms
├── Sounds/                   # Audio asset registers & source files
│   ├── Anomaly/              # Dosimeter ticks, wind-ups, electrical snaps
│   ├── PDA/                  # Icon draw sounds, chat network beeps, warning sirens
│   └── Ambient/              # Global blowout weather rumbles, emissions
├── UI/                       # Graphical layouts and interfaces
│   ├── Layouts/              # PDA.layout, Workbench.layout
│   └── Textures/             # Glassmorphism, cathode buttons, toxic green icons
└── Scripts/                  # Enforce script logic files (.c)
```

---

## 2. Model & Character Blueprints
To bypass official GSC Game World IP restrictions (following **ARMST PLATFORM** best practices), customize or construct high-quality `.fbx` models in Blender and import them through the Workbench Resources manager using the following rules:

### A. Outfits & Exoskeletons (Armor Systems)
Reforger uses the `SCR_InventoryStorageManagerComponent` and custom attachment slots on player skeletons for armor and outfits.
* **Loner Sunrise Suit:**
  - Prefab: `Prefabs/Characters/Outfits/Outfit_Sunrise_Loner.et`
  - Components: `SCR_EquipmentManualAttachmentComponent` bound to character torso and legs slots.
* **Duty/Freedom Heavy Armors:**
  - Prefab: `Prefabs/Characters/Outfits/Outfit_Duty_Heavy.et`
  - Attributes: Modifies base impact protection within the damage manager, adding passive damage reduction multipliers.
* **Exoskeleton Chassis:**
  - Prefab: `Prefabs/Characters/Outfits/Outfit_Exoskeleton.et`
  - Script Hook: Connects to player inventory weight. Decreases running stamina drain by 75% but caps maximum turn rates to simulate hydraulic delay.

---

## 3. Weapons & Stochastic Jamming Configurations
Weapons are defined as prefabs containing a `WeaponComponent`, `MuzzleComponent`, and our custom `SCR_WeaponDurabilityComponent`.

### A. Iconic Weapons Mapping
* **AK-74 (Loner/Military standard):**
  - Path: `Prefabs/Weapons/Rifles/AK74/AK74_Stalker.et`
  - Durability loss per shot: `0.02` (degrades fully after 5,000 rounds)
  - Jamming threshold: `35.0` (stochastic jamming begins below 35% condition)
* **Gauss Rifle (Monolith standard):**
  - Path: `Prefabs/Weapons/Special/Gauss/Gauss_Monolith.et`
  - Durability loss per shot: `0.1` (requires constant battery maintenance)
  - Electromagnetic effect: Targets hit have their electronics (PDA, detectors) wiped or drained.

---

## 4. Audio & Dosimeter Soundscapes
All sound effects must be registered within Enfusion's sound configuration files (`.acp`) to handle 3D spatial attenuation correctly.

### A. PDA Sounds Config
Register inside `Sounds/PDA/PDA_SoundEffects.acp`:
* **pda_open:** Playback on UI toggle (iconic physical draw sound).
* **pda_beep:** Playback on local client PlayerController RPC receipt.
* **blowout_siren_global:** High-intensity spatialized loop broadcasted to base horn speakers.

### B. Detector & Geiger-Counter Sounds
Register inside `Sounds/Geiger/Geiger_SoundEffects.acp`:
* **dosimeter_tick_low:** Ambient clicking. Playback triggered by `SCR_AnomalyDetectorComponent` if an anomaly is within 50 meters.
* **dosimeter_tick_high:** High-frequency crackle. Playback triggered if anomaly is within 10 meters, indicating lethal proximity.

---

## 5. Vehicle Customization
Import custom soviet vehicles, utilizing Reforger's highly realistic physical simulation layer.

### A. UAZ-469 (Motorized Patrols)
* Prefab: `Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_Stalker.et`
* Features: Outfitted with cargo boxes for persistent storage, allowing squads to transport up to 300kg of looted scrap and mutant meats.

### B. Mutated transport (Soviet Kamaz)
* Prefab: `Prefabs/Vehicles/Wheeled/Kamaz/Kamaz_Supply.et`
* Features: Heavy armor paneling to absorb radiation spikes during global blowout transits.
