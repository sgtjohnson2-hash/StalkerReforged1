class SCR_PDANetworkManagerClass : GenericEntityClass
{
}

class SCR_PDANetworkManager : GenericEntity
{
	protected static SCR_PDANetworkManager s_Instance;
	
	[Attribute("300", desc: "Seconds between ambient spoofed PDA messages")]
	protected float m_fAmbientMessageInterval;
	
	protected ref array<string> m_aAmbientMessages = {
		"Checking anomalies near Montignac. Lots of strange readings. - Loner",
		"Anyone trading artifact detectors down in Morton? - Local Scavenger",
		"Heavy mutant activity spotted climbing up to Castle. Stay frosty. - Security Sergeant",
		"Just found a Jellyfish near the Levie military base! Drinks are on me tonight!"
	};

	static SCR_PDANetworkManager GetInstance()
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
		
		// Run loops exclusively on the server (authority)
		if (Replication.IsServer())
		{
			GetGame().GetCallqueue().CallLater(BroadcastAmbientMessage, m_fAmbientMessageInterval * 1000, true);
		}
	}

	void BroadcastAmbientMessage()
	{
		if (!Replication.IsServer()) return;
		if (m_aAmbientMessages.Count() == 0) return;
		
		int randomIndex = Math.RandomInt(0, m_aAmbientMessages.Count());
		string msg = m_aAmbientMessages[randomIndex];
		
		SendNetworkMessage(msg, "Network Channel");
	}
	
	// Called by character death scripts on the server
	void OnCharacterDeath(IEntity victim, IEntity killer, string deathCause)
	{
		if (!Replication.IsServer()) return;
		
		string victimName = "Unknown Stalker";
		
		// Try to extract name from character identity
		SCR_CharacterIdentityComponent identity = SCR_CharacterIdentityComponent.Cast(victim.FindComponent(SCR_CharacterIdentityComponent));
		if (identity) victimName = identity.GetIdentity().GetName();
		
		string msg;
		if (killer && killer != victim)
		{
			string killerName = "Unknown Entity";
			SCR_CharacterIdentityComponent killerIdentity = SCR_CharacterIdentityComponent.Cast(killer.FindComponent(SCR_CharacterIdentityComponent));
			if (killerIdentity) killerName = killerIdentity.GetIdentity().GetName();
			
			msg = "Stalker network reports: " + victimName + " was killed by " + killerName + " (" + deathCause + ").";
		}
		else
		{
			msg = "Stalker network reports: " + victimName + " died to " + deathCause + ".";
		}

		SendNetworkMessage(msg, "Grim Reaper Network");
	}

	void BroadcastBlowoutWarning()
	{
		if (!Replication.IsServer()) return;
		
		string msg = "WARNING: Massive energy spike detected. An emission is imminent. Find deep cover immediately!";
		SendNetworkMessage(msg, "EMERGENCY BROADCAST");
	}

	void SendNetworkMessage(string text, string sender)
	{
		Print("[PDA Server] " + sender + ": " + text);

		if (Replication.IsServer())
		{
			// Replicate to all clients via RPC
			Rpc(RpcDo_ClientReceiveMessage, sender, text);
		}
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_ClientReceiveMessage(string sender, string text)
	{
		SCR_PDA_UI ui = SCR_PDA_UI.GetInstance();
		if (ui)
		{
			ui.ReceiveMessage(sender, text);
		}
	}
}
