class SCR_PlayerFactionManagerComponentClass : ScriptComponentClass
{
}

class SCR_PlayerFactionManagerComponent : ScriptComponent
{
	[Attribute("CIV", desc: "Starting Faction Key for the player")]
	protected string m_sCurrentFaction;

	protected FactionAffiliationComponent m_FactionAffiliation;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_FactionAffiliation = FactionAffiliationComponent.Cast(owner.FindComponent(FactionAffiliationComponent));
		
		if (m_FactionAffiliation)
		{
			// Initialize Loner/Freelance inherently at round start
			JoinFaction(m_sCurrentFaction);
		}
	}

	void JoinFaction(string factionKey)
	{
		if (!m_FactionAffiliation) return;

		// Requires native mapping through FactionManager, conceptually passing string keys to backend system
		Faction faction = GetGame().GetFactionManager().GetFactionByKey(factionKey);
		if (faction)
		{
			m_FactionAffiliation.SetAffiliatedFaction(faction);
			m_sCurrentFaction = factionKey;
			
			string factionName = "";
			if (factionKey == "US") factionName = "NATO MERCENARIES";
			else if (factionKey == "USSR") factionName = "SOVIET MILITARY";
			else if (factionKey == "FIA") factionName = "RENEGADE BANDITS";
			else if (factionKey == "CIV") factionName = "FREELANCE LONER";
			
			Print("Client: Enlistment contract approved. You are now formally affiliated with " + factionName);
			
			SCR_PDA_UI ui = SCR_PDA_UI.GetInstance();
			if (ui) ui.ReceiveMessage("Network", "Your PDA identification tags have been wiped and remapped to " + factionName + " clearance.");
		}
	}

	string GetCurrentFaction()
	{
		return m_sCurrentFaction;
	}
}
