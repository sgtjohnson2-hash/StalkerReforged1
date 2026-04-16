enum EStalkerArchetype
{
	LONER,
	BANDIT,
	MILITARY,
	MERCENARY,
	MONOLITH
};

[BaseContainerProps(configRoot: true)]
class SCR_StalkerArchetypeConfig
{
	[Attribute("1.0", desc: "Multiplier for Greed generation")]
	float m_fGreedMultiplier;

	[Attribute("1.0", desc: "Multiplier for Rest/Tiredness generation")]
	float m_fRestMultiplier;

	[Attribute("0.0", desc: "Inherent bonus bias towards patrolling")]
	float m_fPatrolBias;
	
	[Attribute("0.0", desc: "Inherent bonus bias towards scavenging")]
	float m_fScavengeBias;
}

[BaseContainerProps(configRoot: true)]
class SCR_StalkerArchetypeLibrary
{
	[Attribute("", UIWidgets.Object, desc: "Archetype configurations")]
	ref map<EStalkerArchetype, ref SCR_StalkerArchetypeConfig> m_Archetypes;
	
	static SCR_StalkerArchetypeLibrary GetInstance()
	{
		// In a full implementation this would be fetched from a central manager or resource load.
		// For now we instantiate default values if not loaded.
		return new SCR_StalkerArchetypeLibrary();
	}
	
	// Quick helper to fetch hardcoded defaults if a config file isn't supplied
	SCR_StalkerArchetypeConfig GetConfig(EStalkerArchetype type)
	{
		if (m_Archetypes && m_Archetypes.Contains(type))
		{
			return m_Archetypes.Get(type);
		}
		
		// Fallback Procedural Defaults based on S.T.A.L.K.E.R. lore
		SCR_StalkerArchetypeConfig cfg = new SCR_StalkerArchetypeConfig();
		switch (type)
		{
			case EStalkerArchetype.BANDIT:
				cfg.m_fGreedMultiplier = 2.0; // Very greedy
				cfg.m_fRestMultiplier = 1.0;
				cfg.m_fScavengeBias = 3.0; // Love to search and destroy
				break;
				
			case EStalkerArchetype.MILITARY:
				cfg.m_fGreedMultiplier = 0.5; // Do not care about loot
				cfg.m_fRestMultiplier = 0.8;
				cfg.m_fPatrolBias = 4.0; // Heavy duty patrols
				break;
				
			case EStalkerArchetype.MONOLITH:
				cfg.m_fGreedMultiplier = 0.0; // Fanatics
				cfg.m_fRestMultiplier = 0.1; // Do they even sleep?
				cfg.m_fPatrolBias = 10.0; // Stand guard forever
				break;
				
			case EStalkerArchetype.MERCENARY:
				cfg.m_fGreedMultiplier = 1.5;
				cfg.m_fRestMultiplier = 1.0;
				cfg.m_fPatrolBias = 2.0;
				break;
		}
		
		return cfg;
	}
}
