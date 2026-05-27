class SCR_PlayerSaveManagerClass : GenericEntityClass
{
}

class SCR_PlayerSaveManager : GenericEntity
{
	protected static SCR_PlayerSaveManager s_Instance;

	static SCR_PlayerSaveManager GetInstance()
	{
		return s_Instance;
	}

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
	}

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		s_Instance = this;
		
		// Auto-save all active players every 5 minutes (300000ms) on server authority
		if (Replication.IsServer())
		{
			GetGame().GetCallqueue().CallLater(SaveAllActivePlayers, 300000, true);
		}
	}

	// Server-authoritative save execution for a specific player controller
	void SavePlayer(PlayerController pc)
	{
		if (!Replication.IsServer() || !pc) return;

		int playerId = pc.GetPlayerId();
		string playerName = GetGame().GetPlayerManager().GetPlayerName(playerId);
		if (playerName == "") return;

		IEntity character = pc.GetControlledEntity();
		if (!character) return;

		// Query player components
		SCR_PDATaskManagerComponent pdaMgr = SCR_PDATaskManagerComponent.Cast(pc.FindComponent(SCR_PDATaskManagerComponent));
		SCR_SurvivalManagerComponent survivalMgr = SCR_SurvivalManagerComponent.Cast(character.FindComponent(SCR_SurvivalManagerComponent));
		SCR_RadiationSicknessComponent radMgr = SCR_RadiationSicknessComponent.Cast(character.FindComponent(SCR_RadiationSicknessComponent));
		SCR_PlayerFactionManagerComponent factionMgr = SCR_PlayerFactionManagerComponent.Cast(character.FindComponent(SCR_PlayerFactionManagerComponent));

		int ru = 0;
		if (pdaMgr) ru = pdaMgr.GetPlayerRuBalance();

		float hunger = 100.0;
		float thirst = 100.0;
		float sleep = 100.0;
		if (survivalMgr)
		{
			hunger = survivalMgr.GetHunger();
			thirst = survivalMgr.GetThirst();
			sleep = survivalMgr.GetSleep();
		}

		float rad = 0.0;
		if (radMgr) rad = radMgr.GetRadiationTotal();

		string faction = "CIV";
		if (factionMgr) faction = factionMgr.GetCurrentFaction();

		// Serialize to a custom key-value layout
		string dataString = "RU:" + ru + ";Hunger:" + hunger + ";Thirst:" + thirst + ";Sleep:" + sleep + ";Rad:" + rad + ";Faction:" + faction;

		// Write to server profile directory safely
		string savePath = "$profile:PlayerSaves_" + playerName + ".txt";
		FileIO.OpenFile(savePath, FileMode.WRITE);
		FileIO.Write(dataString);
		FileIO.Close();

		Print("Server RP Database: Saved player progress for " + playerName + " at " + savePath);
	}

	// Server-authoritative load execution for a specific player controller
	void LoadPlayer(PlayerController pc)
	{
		if (!Replication.IsServer() || !pc) return;

		int playerId = pc.GetPlayerId();
		string playerName = GetGame().GetPlayerManager().GetPlayerName(playerId);
		if (playerName == "") return;

		string loadPath = "$profile:PlayerSaves_" + playerName + ".txt";
		if (!FileIO.FileExists(loadPath))
		{
			Print("Server RP Database: No save file found for " + playerName + ". Initializing starting stats.");
			return;
		}

		FileIO.OpenFile(loadPath, FileMode.READ);
		string loadedData;
		FileIO.Read(loadedData);
		FileIO.Close();

		if (loadedData == "") return;

		IEntity character = pc.GetControlledEntity();
		if (!character) return;

		// Instantiating components
		SCR_PDATaskManagerComponent pdaMgr = SCR_PDATaskManagerComponent.Cast(pc.FindComponent(SCR_PDATaskManagerComponent));
		SCR_SurvivalManagerComponent survivalMgr = SCR_SurvivalManagerComponent.Cast(character.FindComponent(SCR_SurvivalManagerComponent));
		SCR_RadiationSicknessComponent radMgr = SCR_RadiationSicknessComponent.Cast(character.FindComponent(SCR_RadiationSicknessComponent));
		SCR_PlayerFactionManagerComponent factionMgr = SCR_PlayerFactionManagerComponent.Cast(character.FindComponent(SCR_PlayerFactionManagerComponent));

		// Custom split parser
		array<string> dataPairs = new array<string>();
		loadedData.Split(";", dataPairs);

		foreach (string pair : dataPairs)
		{
			array<string> kv = new array<string>();
			pair.Split(":", kv);
			if (kv.Count() < 2) continue;

			string key = kv[0];
			string val = kv[1];

			if (key == "RU" && pdaMgr)
			{
				pdaMgr.AddPlayerRuBalance(val.ToInt());
			}
			else if (key == "Hunger" && survivalMgr)
			{
				float diff = val.ToFloat() - survivalMgr.GetHunger();
				survivalMgr.ConsumeFood(diff);
			}
			else if (key == "Thirst" && survivalMgr)
			{
				float diff = val.ToFloat() - survivalMgr.GetThirst();
				survivalMgr.ConsumeDrink(diff);
			}
			else if (key == "Sleep" && survivalMgr)
			{
				float diff = val.ToFloat() - survivalMgr.GetSleep();
				survivalMgr.Rest(diff);
			}
			else if (key == "Rad" && radMgr)
			{
				float diff = val.ToFloat() - radMgr.GetRadiationTotal();
				if (diff > 0) radMgr.AddRadiation(diff);
				else if (diff < 0) radMgr.ReduceRadiation(-diff);
			}
			else if (key == "Faction" && factionMgr)
			{
				factionMgr.JoinFaction(val);
			}
		}

		Print("Server RP Database: Loaded player progress for " + playerName);
	}

	protected void SaveAllActivePlayers()
	{
		if (!Replication.IsServer()) return;

		array<int> playerIds = new array<int>();
		GetGame().GetPlayerManager().GetPlayers(playerIds);

		foreach (int playerId : playerIds)
		{
			PlayerController pc = GetGame().GetPlayerManager().GetPlayerController(playerId);
			if (pc)
			{
				SavePlayer(pc);
			}
		}
	}
}
