using UnrealBuildTool;
using System.Collections.Generic;

public class LibraryOfKnowledgeTarget : TargetRules
{
	public LibraryOfKnowledgeTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("LibraryOfKnowledge");
	}
}
