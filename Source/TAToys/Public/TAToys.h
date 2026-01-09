// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_DELEGATE_FourParams(FOnImportFinished,bool/*Success*/,int/*Index*/,int/*Summary*/,FName/*TextureGroupName*/);

class FTAToysModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/*
	 * TextureImporter
	*/
#pragma region ContentBrowserExtend

protected:
	/**
	 * 总入口，在模块初始化时ViewPathContextMenu添加Extender
	 */
	void InitContentBrowserEntry();

	/**
	 * InitContentBrowserEntry绑定函数用于创建Extender并设置插入点
	 * 设置 UserSelectedPaths
	 */
	TSharedRef<FExtender> AddNewExtenderToPathView(const TArray<FString>& ExtenderNames);

	/**
	 * 输入全部选择的路径，返回其中有效的，待完善
	 * @param InAllPath 全部路径
	 * @return 有效路径
	 */
	TArray<FString> GetValidPath(const TArray<FString>& InAllPath) const;

	/**
	 * 用户选择的路径
	 */
	TArray<FString> UserSelectedPaths = {TEXT("D:\\UEProject\\Assets\\Texture")};

	/**
	 * 在Extender点位添加按钮
	 */
	void AddImportButtonToPathView(FMenuBuilder& MenuBuilder);

	/**
	 * 创建并展示悬浮标签页窗口
	 */
	void OpenImportWindow();

	/**
	 * 纹理导入控件名称
	 */
	const FName TexturesImporterTabName = "TexturesImporter";

	/**
	 * 创建纹理导入控件
	 */
	TSharedRef<SDockTab> CreateNomadSlateContainer(const FSpawnTabArgs& SpawnTabArgs);

	/**
	 * 判断是否打开纹理导入窗口，如果无可用路径时不打开
	 */
	bool CanCreateTextureImporter(const FSpawnTabArgs& SpawnTabArgs) const;

#pragma region FileAndAssetOperations

protected:
	const TSet<FString> SupportFormat{".jpg", ".jpeg", ".png", ".bmp", ".tif", ".tiff", ".exr", ".targa"};
	//比较难确切切出字段，使用TSet没有优势
	const TArray<FString> TextureKeywords{
		"diffuse", "diff", "albedo", "alb", "base", "col", "color", "basecolor", "metallic", "metalness", "metal",
		"mlt", "met", "speculatiry", "specular", "spec", "spc", "roughness", "rough", "rgh", "gloss", "glossy",
		"glossiness", "transmission", "transparency", "trans", "emission", "emissive", "emit", "emm", "opacity", "opac",
		"alpha", "ambient_occlusion", "ao", "occlusion", "bump", "bmp", "height", "displacement", "displace", "disp",
		"dsp", "heightmap", "user", "mask", "normal", "nor", "nrm", "nrml", "norm", "translucency", "sss"
	};
	const TMap<FString, FString> KeywordToTypeMap{
		{"diffuse", "texturesColor"}, {"diff", "texturesColor"}, {"albedo", "texturesColor"}, {"alb", "texturesColor"},
		{"base", "texturesColor"}, {"col", "texturesColor"}, {"color", "texturesColor"}, {"basecolor", "texturesColor"},
		{"metallic", "texturesMetal"}, {"metalness", "texturesMetal"}, {"metal", "texturesMetal"},
		{"mlt", "texturesMetal"}, {"met", "texturesMetal"}, {"speculatiry", "texturesSpecular"},
		{"specular", "texturesSpecular"}, {"spec", "texturesSpecular"}, {"spc", "texturesSpecular"},
		{"roughness", "texturesRough"}, {"rough", "texturesRough"}, {"rgh", "texturesRough"},
		{"gloss", "texturesGloss"}, {"glossy", "texturesGloss"}, {"glossiness", "texturesGloss"},
		{"transmission", "texturesTrans"}, {"transparency", "texturesTrans"}, {"trans", "texturesTrans"},
		{"emission", "texturesEmm"}, {"emissive", "texturesEmm"}, {"emit", "texturesEmm"}, {"emm", "texturesEmm"},
		{"opacity", "texturesAplha"}, {"opac", "texturesAplha"}, {"alpha", "texturesAplha"},
		{"ambient_occlusion", "texturesAO"}, {"ao", "texturesAO"}, {"occlusion", "texturesAO"},
		{"bump", "texturesBump"}, {"bmp", "texturesBump"}, {"height", "texturesBump"}, {"displacement", "texturesDisp"},
		{"displace", "texturesDisp"}, {"disp", "texturesDisp"}, {"dsp", "texturesDisp"}, {"heightmap", "texturesDisp"},
		{"user", "texturesExtra"}, {"mask", "texturesExtra"}, {"normal", "texturesNormal"}, {"nor", "texturesNormal"},
		{"nrm", "texturesNormal"}, {"nrml", "texturesNormal"}, {"norm", "texturesNormal"},
		{"translucency", "texturesSSS"}, {"sss", "texturesSSS"}
	};
	//<TextureName,<TextureType,Path>>
	TMap<FName,TMap<FName,FString>> TextureInfos;

public:
	[[nodiscard]]TArray<FString> GetAllTexturesInPath(const FString& InPath);

	bool GetTextureInfos(const FString& InPath,TArray<FName>& OutTextureNames,TArray<int>& OutCount);
protected:	
	void SortTextures(const TArray<FString>& InImagePaths,TArray<FName>& OutTextureNames,TArray<int>& OutCount);

public:
	bool ImportImages(const FString& InAssetSavingPath,const TArray<FName>& InSelectedTextureGroupName,const FOnImportFinished& ImportCallback);
	
protected:
	FOnImportFinished OnImportFinished;
	
	
#pragma endregion FileAndAssetOperations


#pragma endregion
};
