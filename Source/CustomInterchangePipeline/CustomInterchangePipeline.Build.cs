using UnrealBuildTool;

public class CustomInterchangePipeline : ModuleRules
{
    public CustomInterchangePipeline(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "InterchangeCore",
                "InterchangeEngine",
                "InterchangeFactoryNodes",
                "InterchangeNodes"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "ApplicationCore",
                "InterchangeImport",
                "InputCore",
                "MainFrame",
                "UnrealEd"
            }
        );
    }
}