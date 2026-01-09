// Copyright Epic Games, Inc. All Rights Reserved.

#include "TAToys.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "TexturesImporterSlate.h"

#define LOCTEXT_NAMESPACE "FTAToysModule"

void FTAToysModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	InitContentBrowserEntry();
}

void FTAToysModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}
#pragma region ContentBrowserExtend
void FTAToysModule::InitContentBrowserEntry()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(
		TEXT("ContentBrowser"));
	TArray<FContentBrowserMenuExtender_SelectedPaths>& PathViewExtender = ContentBrowserModule.
		GetAllPathViewContextMenuExtenders();

	PathViewExtender.Add(
		FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this, &FTAToysModule::AddNewExtenderToPathView));
}

TSharedRef<FExtender> FTAToysModule::AddNewExtenderToPathView(const TArray<FString>& InSelectedPaths)
{
	TSharedRef<FExtender> ImportImagesExtenderRef(new FExtender);

	UserSelectedPaths = GetValidPath(InSelectedPaths);
	ImportImagesExtenderRef->AddMenuExtension(TEXT("Explore"), EExtensionHook::Before,
	                                          TSharedPtr<FUICommandList>(),
	                                          FMenuExtensionDelegate::CreateRaw(
		                                          this, &FTAToysModule::AddImportButtonToPathView));
	return ImportImagesExtenderRef;
}

TArray<FString> FTAToysModule::GetValidPath(const TArray<FString>& InAllPath) const
{
	TArray<FString> Results;
	for (const auto& SinglePath : InAllPath)
	{
		if (SinglePath.IsEmpty())
		{
			//UE_LOG(LogTemp, Error, TEXT("The Path [%s] In Selection Is Not Valid Path,Ignore It"), *SinglePath)
			continue;
		}
		UE_LOG(LogTemp, Display, TEXT("Add Path [%s] To Target Path Array"), *SinglePath)
		Results.Add(SinglePath);
	}
	return Results;
}

void FTAToysModule::AddImportButtonToPathView(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(FText::FromString(TEXT("OpenImageImporter")),
	                         FText::FromString("Open a window to choose images  that will be Import"), FSlateIcon(),
	                         FExecuteAction::CreateRaw(this, &FTAToysModule::OpenImportWindow));
}

void FTAToysModule::OpenImportWindow()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TexturesImporterTabName,
	                                                  FOnSpawnTab::CreateRaw(
		                                                  this, &FTAToysModule::CreateNomadSlateContainer),
	                                                  FCanSpawnTab::CreateRaw(
		                                                  this, &FTAToysModule::CanCreateTextureImporter));
	FGlobalTabmanager::Get()->TryInvokeTab(TexturesImporterTabName);
}

TSharedRef<SDockTab> FTAToysModule::CreateNomadSlateContainer(const FSpawnTabArgs& SpawnTabArgs)
{
	TSharedRef<SDockTab> DockTabAsContainer = SNew(SDockTab).TabRole(NomadTab);
	DockTabAsContainer->SetContent(SNew(STexturesImporterSlate).UserSelectedPath(UserSelectedPaths[0]));
	return DockTabAsContainer;
}

bool FTAToysModule::CanCreateTextureImporter(const FSpawnTabArgs& SpawnTabArgs) const
{
	if (UserSelectedPaths.IsEmpty())
	{
		FText TitleText = FText::FromString(TEXT("Warning"));
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromStringView(TEXT("Please Select A Valid Path")), TitleText);
		return false;
	}
	return true;
}

TArray<FString> FTAToysModule::GetAllTexturesInPath(const FString& InPath)
{
	TArray<FString> Results;
	if (!FPaths::DirectoryExists(InPath))
	{
		return Results;
	}
	TArray<FString> AllFiles;
	IPlatformFile& PlatformFileManager = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFileManager.FindFiles(AllFiles, *InPath,NULL);
	UE_LOG(LogTemp, Display, TEXT("Find %d Textures in Path [%s]"), AllFiles.Num(), *InPath);
	if (!AllFiles.IsEmpty())
	{
		for (const FString& File : AllFiles)
		{
			int32 DotIndex = INDEX_NONE;
			if (File.FindLastChar('.', DotIndex))
			{
				FString FileExtension = File;
				FileExtension.RightChopInline(DotIndex);
				if (SupportFormat.Contains(FileExtension))
				{
					UE_LOG(LogTemp, Display, TEXT("Find Image File[%s]"), *File);
					Results.Add(File);
				}
			}
		}
	}

	return Results;
}

bool FTAToysModule::GetTextureInfos(const FString& InPath,TArray<FName>& OutTextureNames,TArray<int>& OutCount)
{
	TArray<FString> AllTextureFilePaths = GetAllTexturesInPath(InPath);
	if (AllTextureFilePaths.IsEmpty())
	{
		return false;
	}
	SortTextures(AllTextureFilePaths,OutTextureNames,OutCount);
	
	return !OutTextureNames.IsEmpty();
}

void FTAToysModule::SortTextures(const TArray<FString>& InImagePaths,TArray<FName>& OutTextureNames,TArray<int>& OutCount)
{
	OutTextureNames.Reset();
	OutCount.Reset();
	TextureInfos.Reset();
	for (const auto& ImagePath : InImagePaths)
	{
		FString BaseFileName = FPaths::GetBaseFilename(ImagePath);
		int32 UnderscoreIndex = INDEX_NONE;
		//如果不包含_一定不对
		if (!BaseFileName.FindChar('_', UnderscoreIndex))
		{
			UE_LOG(LogTemp, Display, TEXT("Find Null Underscore in File %s"), *ImagePath);
			continue;
		};
		FString TextureNameStr = BaseFileName.Left(UnderscoreIndex);
		int32 KeywordIndex = 0;
		for (const auto& TypeKeyword : TextureKeywords)
		{
			KeywordIndex = BaseFileName.Find(TypeKeyword, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
			if (KeywordIndex != INDEX_NONE)
			{
				UE_LOG(LogTemp, Display, TEXT("Find{%s} in {%s} At:%d"), *TypeKeyword, *ImagePath, KeywordIndex);
				FName TextureName=FName(TextureNameStr);
				if (TextureInfos.Contains(TextureName))
				{
					TextureInfos[TextureName].Emplace(KeywordToTypeMap[TypeKeyword], ImagePath);
				}
				else
				{
					TextureInfos.Add(TextureName).Emplace(KeywordToTypeMap[TypeKeyword], ImagePath);
					OutTextureNames.Add(TextureName);
					UE_LOG(LogTemp, Display, TEXT("Add New Texture{%s}"), *TextureNameStr);
				}
				break;
			}
		}
		//这里可以做进一步排除获得更精准的纹理名称，找到第二个_和关键词对比
		//int32 SecondUnderscoreIndex = BaseFileName.Find("_", ESearchCase::IgnoreCase, ESearchDir::FromStart, UnderscoreIndex + 1);
	}
	//TMap无序
	for (const auto & TextureGroupName:OutTextureNames)
	{
		OutCount.Add(TextureInfos[TextureGroupName].Num());
	}
	return;
}

bool FTAToysModule::ImportImages(const FString& InAssetSavingPath, const TArray<FName>& InSelectedTextureGroupName,
	const FOnImportFinished& ImportCallback)
{
	OnImportFinished=ImportCallback;
	int32 TextureGroupCount=0;
	int32 TotalTextureCount=InSelectedTextureGroupName.Num();
	//& AssetToolsModule=FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	for (const auto& TextureGroupName : InSelectedTextureGroupName)
	{
		TArray<FString> TexturePaths;
		TextureInfos[TextureGroupName].GenerateValueArray(TexturePaths);
		FAssetToolsModule::GetModule().Get().ImportAssets(TexturePaths,InAssetSavingPath);
		// for (const auto& TextureNameAndPath : )
		// {
		// 	UE_LOG(LogTemp,Display,TEXT("Import{%s} to {%s}"), *TextureNameAndPath.Value,*InAssetSavingPath);
		//
		// }
		OnImportFinished.ExecuteIfBound(true,TextureGroupCount,TotalTextureCount,TextureGroupName);
		TextureGroupCount++;
	}
	return true;
}
#pragma endregion

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTAToysModule, TAToys)
