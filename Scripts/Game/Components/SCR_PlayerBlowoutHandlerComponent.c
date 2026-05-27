class SCR_PlayerBlowoutHandlerComponentClass : ScriptComponentClass
{
}

class SCR_PlayerBlowoutHandlerComponent : ScriptComponent
{
	protected IEntity m_Owner;
	protected bool m_bInPeak;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_Owner = owner;
		m_bInPeak = false;
	}

	// Called on the server to start the warning phase
	void OnWarningPhase()
	{
		if (!Replication.IsServer()) return;
		
		// Send RPC to the owning client to play warning effects
		Rpc(RpcDo_ClientWarningPhase);
	}

	// Called on the server to start the peak phase
	void OnPeakPhase()
	{
		if (!Replication.IsServer()) return;
		
		m_bInPeak = true;
		
		// Start server-side raycast checks every 2 seconds
		GetGame().GetCallqueue().CallLater(CheckDamageServer, 2000, true);
		
		// Send RPC to client to start screen shakes and rumbles
		Rpc(RpcDo_ClientPeakPhase);
	}

	// Called on the server to end the blowout
	void OnEndPhase()
	{
		if (!Replication.IsServer()) return;
		
		m_bInPeak = false;
		GetGame().GetCallqueue().Remove(CheckDamageServer);
		
		// Send RPC to client to stop effects
		Rpc(RpcDo_ClientEndPhase);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_ClientWarningPhase()
	{
		Print("Client: Warning Phase. Sirens playing locally.");
		// Play local siren sound or trigger client UI notification here
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_ClientPeakPhase()
	{
		Print("Client: Peak Phase. Screen shaking, blowout roar rumble active.");
		// Play local blowout roar rumble and trigger screen shake here
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_ClientEndPhase()
	{
		Print("Client: End Phase. Blowout over, sky clearing.");
		// Restore normal client visuals and sounds
	}

	protected void CheckDamageServer()
	{
		if (!m_bInPeak || !m_Owner) return;

		// Server-side vertical raycast
		vector mat[4];
		m_Owner.GetTransform(mat);
		
		vector startPoint = mat[3] + "0 1.8 0"; // Raycast starts at head height
		vector endPoint = startPoint + "0 30 0";  // Cast 30m straight up

		BaseWorld world = m_Owner.GetWorld();
		
		// TracePosition checks collisions. Returns true if a collision (roof) is hit.
		bool hit = world.TracePosition(startPoint, endPoint, null, null);

		if (!hit)
		{
			// No roof found -> player is outside, apply massive damage
			SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(m_Owner.FindComponent(SCR_CharacterDamageManagerComponent));
			if (damageManager && damageManager.GetHealth() > 0)
			{
				HitZone defaultHZ = damageManager.GetDefaultHitZone();
				if (defaultHZ)
				{
					// Inflict high damage (25% health per tick) so they die in 8 seconds if exposed
					defaultHZ.Damage(25.0);
					Print("Server: Player " + m_Owner.GetName() + " took blowout radiation damage.");
				}
			}
		}
	}
}
