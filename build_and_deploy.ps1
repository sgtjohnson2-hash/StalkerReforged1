# Arma Reforger S.T.A.L.K.E.R. Build & Deployment Automator
# This script automates copying files into the Reforger Workbench addons folder and running the Dedicated Server.

$ErrorActionPreference = "Stop"

Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host "    S.T.A.L.K.E.R. RP ARMA REFORGER BUILD & DEPLOYER      " -ForegroundColor Green
Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host "This script will copy your scripts into the Workbench and deploy the server." -ForegroundColor White

# 1. Locate local Reforger Workbench Addons Folder
$UserDocs = [System.IO.Path]::Combine([System.Environment]::GetFolderPath('MyDocuments'), "My Games\ArmaReforgerWorkbench\addons")
$ModName = "Stalker_Anomalies_Emissions"
$TargetAddonPath = Join-Path $UserDocs $ModName

Write-Host "`n[1/3] Locating Workbench Addons Folder..." -ForegroundColor Yellow

if (-not (Test-Path $UserDocs)) {
    Write-Host "Workbench directory not found at $UserDocs, generating folder structure..." -ForegroundColor White
    New-Item -ItemType Directory -Force -Path $UserDocs | Out-Null
}

# 2. Synchronize Addon Files from Workspace to Workbench
Write-Host "`n[2/3] Copying framework files to Reforger Workbench Addon: $ModName..." -ForegroundColor Yellow

$SourcePath = "e:\Stalker Reforged"
$FoldersToSync = @("Scripts", "Prefabs", "UI", "Sounds")

# Create target addon directory
New-Item -ItemType Directory -Force -Path $TargetAddonPath | Out-Null

# Copy addon.gproj
Copy-Item -Path "$SourcePath\addon.gproj" -Destination "$TargetAddonPath\addon.gproj" -Force

# Synchronize each directory
foreach ($Folder in $FoldersToSync) {
    $SourceFolder = Join-Path $SourcePath $Folder
    $TargetFolder = Join-Path $TargetAddonPath $Folder
    
    if (Test-Path $SourceFolder) {
        Write-Host "Copying $Folder folder..." -ForegroundColor White
        Copy-Item -Path $SourceFolder -Destination $TargetAddonPath -Recurse -Force
    }
}

Write-Host "Success: Framework addon synchronized at: $TargetAddonPath" -ForegroundColor Green
Write-Host "Now, when you open ARMA Reforger Tools (Workbench), the mod will compile automatically!" -ForegroundColor Green

# 3. Setup and Boot the Local Dedicated Server
Write-Host "`n[3/3] Running Dedicated Server Auto-Deployer..." -ForegroundColor Yellow

# Trigger our server installer script
$InstallerScript = "$SourcePath\deploy_server.ps1"
if (Test-Path $InstallerScript) {
    powershell -File $InstallerScript
}

Write-Host "`n==========================================================" -ForegroundColor Green
Write-Host "             BUILD & DEPLOYMENT COMPLETE!                 " -ForegroundColor Green
Write-Host "==========================================================" -ForegroundColor Green
Write-Host "1. Launch ARMA Reforger Tools (Workbench) to compile." -ForegroundColor Cyan
Write-Host "2. Double-click C:\ArmaReforgerServer\Start_STALKER_Server.bat to run." -ForegroundColor Cyan
Write-Host "==========================================================" -ForegroundColor Green
