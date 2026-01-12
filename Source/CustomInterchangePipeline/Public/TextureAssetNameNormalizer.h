// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InterchangePipelineBase.h"
#include "TextureAssetNameNormalizer.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMINTERCHANGEPIPELINE_API UTextureAssetNameNormalizer : public UInterchangePipelineBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomTexture2D",
		meta = (StandAlonePipelineProperty = "True", PipelineInternalEditionData = "True"))
	FString PipelineDisplayName;

	virtual void ExecutePipeline(UInterchangeBaseNodeContainer* BaseNodeContainer,
	                             const TArray<UInterchangeSourceData*>& SourceDatas,
	                             const FString& ContentBasePath) override;

	virtual bool CanExecuteOnAnyThread(EInterchangePipelineTask PipelineTask) override { return true; }

protected:
	FString GetNormalizedName(const FString& InTextureNames);

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
};
