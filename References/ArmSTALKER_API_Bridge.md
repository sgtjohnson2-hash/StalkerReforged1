# ArmSTALKER API Bridging & Integration Blueprint

This document details how our server-authoritative, highly optimized Enforce Script systems (`.c` components) map to and can bridge with the open-source codebase of the **ARMST PLATFORM (ArmSTALKER)**.

---

## 1. Bridging Legacy Concepts to Native Enfusion
The ARMST PLATFORM (originally built for Arma 3 SQF procedural paradigms) relies on loop-polling and global arrays. Under Reforger's Enfusion Engine, we map these legacy behaviors to **native, component-based, event-driven architecture**:

| Legacy ARMST Concept (Arma 3 SQF) | Native Stalker Reforged Approach (Enfusion Enforce Script) |
| :--- | :--- |
| **Starvation Loops (`fn_starvation.sqf`)** | Server-authoritative `SCR_SurvivalManagerComponent` with low-overhead timers and direct torso hitzone `Damage()` methods. |
| **Anomaly Area Triggers (`fn_anomElectra.sqf`)** | Spatial trigger zones querying hit entities on contact, dealing server-replicated, hitzone-targeted thermo/shock damage. |
| **Global Loot Lists (`fn_lootGen.sqf`)** | Configuration-driven `SCR_StashLootTable` and server-authoritative spawner injection loops. |
| **PDA Chat Channels (`fn_pdaChat.sqf`)** | Client-Server RPC broadcasts (`RpcDo_ClientReceiveMessage`) dispatched to individual PlayerController `SCR_PDA_UI` widgets. |

---

## 2. Incorporating ArmSTALKER Logic

### A. Dynamic A-Life (Autonomous NPC Lifecycles)
The ARMST mod utilizes a sophisticated AI loop called **A-Life**, which directs mutant packs and stalker factions to wander, sleep, hunt, and seek cover dynamically.
* **Our Bridge:** We carry this paradigm into [SCR_StalkerAIBrainComponent.c](file:///e:/Stalker%20Reforged/Scripts/Game/Components/SCR_StalkerAIBrainComponent.c) and [SCR_AdvancedCombatAIComponent.c](file:///e:/Stalker%20Reforged/Scripts/Game/Components/SCR_AdvancedCombatAIComponent.c).
* **AI Suppression Dynamics:** ArmSTALKER's suppression mechanics are integrated through our server-authoritative bullet snap queries, which actively modify AI navigation meshes to force tactical bounding cover behaviors when under intense fire.

### B. Blowout Raycasting (Safe Shelter Logic)
In legacy systems, players were protected from blowouts by defining complex 2D poly-markers around safe zones.
* **Our Bridge:** We use our advanced server-side vertical raycast trace (`world.TracePosition`), which checks collisions straight up from the character's head bone. This dynamically identifies valid ceiling shelter geometry, permitting physical freedom of shelter construction (e.g. hiding under concrete bridges or inside vehicle garages) anywhere on the map!

---

## 3. Reference Repositories for S.T.A.L.K.E.R. RP Assets
To gather further asset references, configurations, and scripts for your Alpha, check out the following community resources:

1. **ArmSTALKER GIT Repository (APL-SA License):**
   - URL: `https://github.com/romzet/ArmSTALKER_git`
   - Use: Study their scripting for mutant behaviors, anomaly placements, and inventory layout formats.
2. **ARMST Platform Official Portal:**
   - URL: `https://armaplatform.com`
   - Use: Access documentation, server deployment scripts, and community asset conversion tools.
