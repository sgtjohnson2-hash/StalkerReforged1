[EntityEditorProps(category: "GameScripted/Systems", description: "Handles server-wide emission/blowout event.", color: "255 0 0 255")]
class SCR_BlowoutSystemClass : GenericEntityClass
{
}

class SCR_BlowoutSystem : GenericEntity
{
	[Attribute("1800", desc: "Time between blowouts in seconds")]
	protected float m_fBlowoutInterval;

	protected TimeAndWeatherManagerEntity m_WeatherManager;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
	}

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		// Run only on Server
		if (!Replication.IsServer()) return;

		GetGame().GetCallqueue().CallLater(StartBlowoutPhase1, m_fBlowoutInterval * 1000, false);
		m_WeatherManager = GetGame().GetTimeAndWeatherManager();
	}

	protected void StartBlowoutPhase1()
	{
		Print("Server: SCR_BlowoutSystem: Phase 1 (Warning) Started");
		
		if (m_WeatherManager)
		{
			// Darken skies, increase overcast, rain, and fog on server (replicates automatically)
			m_WeatherManager.SetOvercast(1.0);
			m_WeatherManager.SetRainState(1.0);
			m_WeatherManager.SetFogState(0.5);
		}
		
		// Notify all players (warning sirens, chat notification)
		array<int> playerIds = new array<int>();
		GetGame().GetPlayerManager().GetPlayers(playerIds);
		foreach (int playerId : playerIds)
		{
			IEntity playerChar = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
			if (playerChar)
			{
				SCR_PlayerBlowoutHandlerComponent handler = SCR_PlayerBlowoutHandlerComponent.Cast(playerChar.FindComponent(SCR_PlayerBlowoutHandlerComponent));
				if (handler) handler.OnWarningPhase();
			}
		}
		
		// Trigger active warning PDA network messages
		SCR_PDANetworkManager network = SCR_PDANetworkManager.GetInstance();
		if (network)
		{
			network.BroadcastBlowoutWarning();
		}
		
		GetGame().GetCallqueue().CallLater(StartBlowoutPhase2, 120000, false); // 2 minutes warning
	}

	protected void StartBlowoutPhase2()
	{
		Print("Server: SCR_BlowoutSystem: Phase 2 (Peak) Started");
		
		if (m_WeatherManager)
		{
			m_WeatherManager.SetFogState(0.9);
			m_WeatherManager.SetRainState(1.0);
		}
		
		// Notify all players to begin damage checks and play rumbles
		array<int> playerIds = new array<int>();
		GetGame().GetPlayerManager().GetPlayers(playerIds);
		foreach (int playerId : playerIds)
		{
			IEntity playerChar = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
			if (playerChar)
			{
				SCR_PlayerBlowoutHandlerComponent handler = SCR_PlayerBlowoutHandlerComponent.Cast(playerChar.FindComponent(SCR_PlayerBlowoutHandlerComponent));
				if (handler) handler.OnPeakPhase();
			}
		}
		
		GetGame().GetCallqueue().CallLater(EndBlowout, 60000, false); // 1 minute peak duration
	}

	protected void EndBlowout()
	{
		Print("Server: SCR_BlowoutSystem: Blowout Ended");
		
		if (m_WeatherManager)
		{
			m_WeatherManager.SetOvercast(0.0);
			m_WeatherManager.SetRainState(0.0);
			m_WeatherManager.SetFogState(0.0);
		}
		
		// Notify all players to calm atmospheric effects and end damage checks
		array<int> playerIds = new array<int>();
		GetGame().GetPlayerManager().GetPlayers(playerIds);
		foreach (int playerId : playerIds)
		{
			IEntity playerChar = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
			if (playerChar)
			{
				SCR_PlayerBlowoutHandlerComponent handler = SCR_PlayerBlowoutHandlerComponent.Cast(playerChar.FindComponent(SCR_PlayerBlowoutHandlerComponent));
				if (handler) handler.OnEndPhase();
			}
		}

		// Queue next blowout
		GetGame().GetCallqueue().CallLater(StartBlowoutPhase1, m_fBlowoutInterval * 1000, false);
	}
}
