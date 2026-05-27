# 📟 S.T.A.L.K.E.R. RP Reforger Server — Simple Setup Guide

Setting up your dedicated S.T.A.L.K.E.R. Role Play server is incredibly simple! We have automated 100% of the complicated parts. Just follow these 3 childishly easy steps!

---

## ⚡ The 3-Step Single-Click Setup

### Step 1: Run the Auto-Installer Script
We wrote an automated PowerShell installer that does all the work for you. It builds folder directories, installs Valve's SteamCMD tool, configures game mods, and generates launch shortcuts!

1. Open **Windows PowerShell** as Administrator (Click Start, type `PowerShell`, right-click it, and select **Run as Administrator**).
2. Copy and paste this single command into PowerShell and hit **Enter**:
   ```powershell
   Set-ExecutionPolicy Bypass -Scope Process -Force; powershell -File "e:\Stalker Reforged\deploy_server.ps1"
   ```
3. Watch the script run! It will create the server files inside `C:\ArmaReforgerServer` in about 15 seconds.

---

### Step 2: Start Your Server!
1. Open the folder: **`C:\ArmaReforgerServer`**
2. Double-click on the file named: **`Start_STALKER_Server.bat`**
3. A black command window will open. **That is it!** 
   - *On the first launch, it will automatically download the entire official Arma Reforger Dedicated Server platform (approx. 2.5 GB) directly from Steam. Once the download finishes, the server will boot up and load the Stalker Reforged framework automatically!*

---

### Step 3: Open Your Ports (So Friends Can Join)
To let players connect to your server, you need to open two ports on your home internet router (Port Forwarding):
* **Port 2001 (UDP)** - Primary game connection port.
* **Port 17777 (UDP)** - Steam query connection port.

*If you do not know how to port forward, check out [PortForward.com](https://portforward.com) or contact your internet provider!*

---

## 🛠️ How to Customize Your Server Name
Want to change the name that appears in the Reforger server list?
1. Open the file **`C:\ArmaReforgerServer\Configs\server.json`** in Notepad.
2. Find this line:
   ```json
   "name": "[US/EU] S.T.A.L.K.E.R. RP - Gamma Overhaul Alpha"
   ```
3. Change it to your own server name (e.g. `"My Custom Stalker Zone RP!"`).
4. Save the file and restart the server!
