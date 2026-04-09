[EntityEditorProps(category: "GameScripted/Systems", description: "Handles server-wide emission/blowout event.", color: "255 0 0 255")]
class SCR_BlowoutSystemClass : GenericEntityClass
{
}

class SCR_BlowoutSystem : GenericEntity
{
	[Attribute("1800", desc: "Time between blowouts in seconds")]
	protected float m_fBlowoutInterval;

	protected TimeAndWeatherManagerEntity m_WeatherManager;

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		// Run only on server
		if (!GetGame().GetDefaultScriptInvoker(ScriptInvoker.GetDefaultScriptInvokerEvent(DefaultScriptInvokerEvent.POST_INIT))) return;

		GetGame().GetCallqueue().CallLater(StartBlowoutPhase1, m_fBlowoutInterval * 1000, false);
		
		m_WeatherManager = GetGame().GetTimeAndWeatherManager();
	}

	protected void StartBlowoutPhase1()
	{
		Print("SCR_BlowoutSystem: Phase 1 (Warning) Started");
		
		if (m_WeatherManager)
		{
			// Darken skies, increase overcast
			m_WeatherManager.SetOvercast(1.0);
			m_WeatherManager.SetRainState(1.0);
			m_WeatherManager.SetFogState(0.5);
		}
		
		// RPC to clients to play sirens
		Rpc(RpcDo_ClientWarningPhase);
		
		GetGame().GetCallqueue().CallLater(StartBlowoutPhase2, 120000, false); // 2 minutes until peak
	}

	protected void StartBlowoutPhase2()
	{
		Print("SCR_BlowoutSystem: Phase 2 (Peak) Started");
		
		// Extreme weather shift (red skies placeholder logic here)
		
		Rpc(RpcDo_ClientPeakPhase);
		
		GetGame().GetCallqueue().CallLater(EndBlowout, 60000, false); // 1 min peak duration
	}

	protected void EndBlowout()
	{
		Print("SCR_BlowoutSystem: Blowout Ended");
		
		if (m_WeatherManager)
		{
			m_WeatherManager.SetOvercast(0.0);
			m_WeatherManager.SetRainState(0.0);
			m_WeatherManager.SetFogState(0.0);
		}
		
		Rpc(RpcDo_ClientEndPhase);

		// Queue next blowout
		GetGame().GetCallqueue().CallLater(StartBlowoutPhase1, m_fBlowoutInterval * 1000, false);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_ClientWarningPhase()
	{
		// Play siren sound from player handler
		SCR_PlayerBlowoutHandlerComponent handler = SCR_PlayerBlowoutHandlerComponent.GetInstance();
		if (handler) handler.OnWarningPhase();
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_ClientPeakPhase()
	{
		// Play massive blowout rumble, start damage check
		SCR_PlayerBlowoutHandlerComponent handler = SCR_PlayerBlowoutHandlerComponent.GetInstance();
		if (handler) handler.OnPeakPhase();
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_ClientEndPhase()
	{
		// Restore normal PFX
		SCR_PlayerBlowoutHandlerComponent handler = SCR_PlayerBlowoutHandlerComponent.GetInstance();
		if (handler) handler.OnEndPhase();
	}
}
