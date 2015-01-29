// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using System.IO;

namespace UnrealBuildTool.Rules
{
	public class KBLightShow : ModuleRules
	{
        public KBLightShow(TargetInfo Target)
        {
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
                    "Engine",
                    "UnrealTournament",
					"InputCore",
					"SlateCore",
				}
				);

            var LEDPath = Path.Combine("..", "..", "UnrealTournament", "Plugins", "KBLightShow", "Source", "lib");
            
            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                LEDPath = Path.Combine(LEDPath, "x64");
            }
            else if (Target.Platform == UnrealTargetPlatform.Win32)
            {
                LEDPath = Path.Combine(LEDPath, "x86");
            }
                
            var LEDLibPath = Path.Combine(LEDPath, "LogitechLEDLib.lib");
            
			// Lib file
            PublicLibraryPaths.Add(LEDPath);
            PublicAdditionalLibraries.Add(LEDLibPath);
		}
	}
}