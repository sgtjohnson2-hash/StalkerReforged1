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

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		s_Instance = this;
		
		// Start ambient network loop for random chatter
		GetGame().GetCallqueue().CallLater(BroadcastAmbientMessage, m_fAmbientMessageInterval * 1000, true);
	}

	void BroadcastAmbientMessage()
	{
		if (m_aAmbientMessages.Count() == 0) return;
		
		int randomIndex = Math.RandomInt(0, m_aAmbientMessages.Count());
		string msg = m_aAmbientMessages[randomIndex];
		
		SendNetworkMessage(msg, "Network Channel");
	}
	
	// Called by character death scripts (REAL AI death tracking)
	void OnCharacterDeath(IEntity victim, IEntity killer, string deathCause)
	{
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

	// This function can be called by SCR_PlayerBlowoutHandlerComponent or the Emission GameMode
	void BroadcastBlowoutWarning()
	{
		string msg = "WARNING: Massive energy spike detected. An emission is imminent. Find deep cover immediately!";
		SendNetworkMessage(msg, "EMERGENCY BROADCAST");
	}

	protected void SendNetworkMessage(string text, string sender)
	{
		// Native print for debugging
		Print("[PDA] " + sender + ": " + text);

		// If a UI manager exists, dispatch the text to it
		SCR_PDA_UI ui = SCR_PDA_UI.GetInstance();
		if (ui)
		{
			ui.ReceiveMessage(sender, text);
		}
	}
}
