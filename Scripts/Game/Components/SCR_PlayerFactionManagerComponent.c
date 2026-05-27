class SCR_PlayerFactionManagerComponentClass : ScriptComponentClass
{
}

class SCR_PlayerFactionManagerComponent : ScriptComponent
{
	[Attribute("CIV", desc: "Starting Faction Key for the player")]
	[RplProp()]
	protected string m_sCurrentFaction;

	protected FactionAffiliationComponent m_FactionAffiliation;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_FactionAffiliation = FactionAffiliationComponent.Cast(owner.FindComponent(FactionAffiliationComponent));
		
		if (m_FactionAffiliation && Replication.IsServer())
		{
			// Initialize starting faction on the server
			ProcessJoinFactionServer(m_sCurrentFaction);
		}
	}

	void JoinFaction(string factionKey)
	{
		if (Replication.IsClient())
		{
			Rpc(RpcServer_JoinFaction, factionKey);
		}
		else
		{
			ProcessJoinFactionServer(factionKey);
		}
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcServer_JoinFaction(string factionKey)
	{
		ProcessJoinFactionServer(factionKey);
	}

	protected void ProcessJoinFactionServer(string factionKey)
	{
		if (!m_FactionAffiliation) return;

		Faction faction = GetGame().GetFactionManager().GetFactionByKey(factionKey);
		if (faction)
		{
			m_FactionAffiliation.SetAffiliatedFaction(faction);
			m_sCurrentFaction = factionKey;
			Replication.BumpMe();
			
			string factionName = factionKey;
			if (factionKey == "US") factionName = "NATO MERCENARIES";
			else if (factionKey == "USSR") factionName = "SOVIET MILITARY";
			else if (factionKey == "FIA") factionName = "RENEGADE BANDITS";
			else if (factionKey == "CIV") factionName = "FREELANCE LONER";
			
			Print("Server: Enlistment contract approved for Player. Affiliation: " + factionName);
			
			SCR_PDANetworkManager network = SCR_PDANetworkManager.GetInstance();
			if (network)
			{
				network.SendNetworkMessage("PDA identification tags wiped and remapped to " + factionName + " clearance.", "Network");
			}
		}
	}

	string GetCurrentFaction()
	{
		return m_sCurrentFaction;
	}
}
