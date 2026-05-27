# Stalker Reforged: The GAMMA Overhaul

This repository contains a comprehensive, ground-up remaster of classic S.T.A.L.K.E.R. mechanics built natively for the **Enfusion Engine**, transforming Arma Reforger into a hardcore Zone survival and Role Play experience. 

Utilizing highly optimized, component-based Enforce Script (`.c`) instead of legacy SQF loops, this project brings S.T.A.L.K.E.R. GAMMA survival mechanics, fully procedural anomalies, tactical combat AI OODA loops, and a complete multiplayer roleplay economy.

---

## 🩸 Extensive Core Features

### 🩸 Core Survival & Medical Engineering
* **Overhauled Medical (`SCR_GammaMedicalItemComponent`)**: Heal-over-Time loops targeting standard character hitzones rather than instant placebo heals. Incorporates a persistent server-authoritative `SCR_RadiationSicknessComponent` that scales ambient mSv damage.
* **Physical Needs (`SCR_SurvivalManagerComponent`)**: Server-replicated stats that slowly drain Hunger, Thirst, and Sleep, applying default hitzone damage if neglected.
* **Weapon Degradation (`SCR_WeaponDurabilityComponent`)**: Server-authoritative gun wear that correlates with shots fired, triggering stochastic jams below 35% condition.
* **Workbench & Scrap Economy (`SCR_RepairSystemComponent`)**: Actively scans inventories for scrap metal parts or weapon kits on the server, consumes them, and restores weapon durability.

### 🧠 Tactical AI & Environmental Hazards
* **Advanced Combat AI OODA Loops (`SCR_AdvancedCombatAIComponent`)**: Active server-side sphere scans detect hostiles, triggering realistic reaction OODA delays and bounding flanking maneuvers.
* **Component Anomalies (`SCR_AnomalyComponent`)**: Server-authoritative triggers for Electra, Burner, and Teleport anomalies dealing physical hitzone damage and teleportation vectors.
* **Environmental Blowouts (`SCR_BlowoutSystem` & `SCR_PlayerBlowoutHandlerComponent`)**: Automated weather climate events that synchronize rain and fog. The server raycasts above characters, siphoning health if caught outside structures.

---

## 💰 Premium Roleplay Server Economy (New!)

To support an immersive S.T.A.L.K.E.R. Role Play server, we implemented four major systems:
1. **💊 Drug Synthesis & Buffs (`SCR_DrugSystemComponent`)**: Consuming processed drugs grants active combat buffs (Stamina Boost or Health Regeneration) while raising toxicity. Exceeding toxicity thresholds triggers lethal overdose states.
2. **🔑 Safehouses & Land Claims (`SCR_PropertyClaimComponent`)**: Secure rent purchases. Door locks are checked on server authority, permitting access only to the registered tenant or inventory keycard holders.
3. **🌱 Farmland Crops Farming (`SCR_FarmingComponent`)**: Plant crop seeds, water the soil, and monitor development stages (Seed -> Sprout -> Mature -> Harvest Ready) on the server.
4. **⚖️ Central Economy Merchant (`SCR_EconomyManager`)**: Dynamic transaction price calculations based on player faction affiliation (e.g. Military receives discounts at Military traders, but is charged extreme prices by Bandits). Handles scrap conversions.

---

## 👾 Custom Mutant AI Behavior (New!)

* **Snork Leap Attack (`SCR_StalkerMutantBrainComponent`)**: When within 15 meters of player characters, the Snork lunges forward through the air using a physical velocity impulse, dealing 35 HP claw strike damage on impact.
* **Bloodsucker Invisibility (`SCR_StalkerMutantBrainComponent`)**: Activates cloaking beyond 10 meters, broadcasting client RPCs to hide its mesh. When within 3 meters, it uncloaks, screams, and executes a vampire life siphon to heal itself!

---

## 📟 Custom S.T.A.L.K.E.R. PDA UI Menu (New!)

We built a native Enfusion UI controller **`SCR_StalkerPDAMenu.c`** inheriting from `ChimeraMenuBase` to govern custom layouts:
* **Tasks Log:** Renders active assignments and rewards.
* **Chat Logs:** Real-time replicated chat feeds from the server network.
* **Factions Board:** Shows dynamic territory control counters.

---

## 🚀 Childishly Simple Server Installation

We automated 100% of the Dedicated Server setup! 
1. Open **Windows PowerShell** as Administrator and execute this single-click setup:
   ```powershell
   Set-ExecutionPolicy Bypass -Scope Process -Force; powershell -File "e:\Stalker Reforged\deploy_server.ps1"
   ```
2. Navigate to the newly created folder `C:\ArmaReforgerServer` and double-click **`Start_STALKER_Server.bat`**.
3. That is it! The launcher will automatically fetch files from Steam and boot your server.
4. For customized server names and detailed configurations, read the **[SERVER_SETUP.md](file:///e:/Stalker%20Reforged/SERVER_SETUP.md)** guide.
