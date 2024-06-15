using UnrealBuildTool;
using System.Collections.Generic;

public class LibraryOfKnowledgeEditorTarget : TargetRules
{
	public LibraryOfKnowledgeEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("LibraryOfKnowledge");
	}
}
