// Fill out your copyright notice in the Description page of Project Settings.


#include "TextureAssetNameNormalizer.h"

//#include "InterchangeTexture2DFactoryNode.h"
#include "InterchangeTexture2DFactoryNode.h"
#include "InterchangeTexture2DNode.h"

void UTextureAssetNameNormalizer::ExecutePipeline(UInterchangeBaseNodeContainer* BaseNodeContainer,
                                                  const TArray<UInterchangeSourceData*>& SourceDatas, const FString& ContentBasePath)
{
	TArray<FString> TexturesFactoryNodeNames;
	BaseNodeContainer->GetNodes(UInterchangeTexture2DFactoryNode::StaticClass(), TexturesFactoryNodeNames);
	if (TexturesFactoryNodeNames.Num()==1)
	{
		UInterchangeTexture2DFactoryNode* Texture2DNode=Cast<UInterchangeTexture2DFactoryNode>(BaseNodeContainer->GetFactoryNode(TexturesFactoryNodeNames[0]));
		if (Texture2DNode)
		{
			FString AssetLabel=Texture2DNode->GetAssetName();
			UE_LOG(LogTemp,Display,TEXT("Current Asset Name： %s"),*AssetLabel);
			/*TArray<FString> ParsedNameSegments;
			AssetLabel.ParseIntoArray(ParsedNameSegments, TEXT("_"));*/
			Texture2DNode->SetAssetName(GetNormalizedName(AssetLabel));
		}
	}
	else
	{
		UE_LOG(LogTemp,Error,TEXT("Only one texture factory node allowed!"));
	}
	Super::ExecutePipeline(BaseNodeContainer, SourceDatas, ContentBasePath);
	
}

FString UTextureAssetNameNormalizer::GetNormalizedName(const FString& InTextureNames)
{
	FString NormalizedName=InTextureNames;
	int32 FirstUnderscoreIndex;
	NormalizedName.FindChar('_',FirstUnderscoreIndex);
	if (FirstUnderscoreIndex==INDEX_NONE)
		return NormalizedName;
	//查找第二个下划线作为切分字段
	int32 SecondUnderscoreIndex=NormalizedName.Find("_",ESearchCase::IgnoreCase,ESearchDir::FromStart,FirstUnderscoreIndex+1);
	//只有一个下划线作为分割，后边可能是纹理类型
	if (SecondUnderscoreIndex==INDEX_NONE)
	{
		NormalizedName=InTextureNames.Left(FirstUnderscoreIndex);
	}
	else
	{
		NormalizedName=InTextureNames.Left(SecondUnderscoreIndex);
	}
	//找纹理类型
	int32 KeywordIndex=INDEX_NONE;
	const FString SearchingTailStr=InTextureNames.RightChop(FMath::Max(SecondUnderscoreIndex,FirstUnderscoreIndex)+1);
	FString TextureType="";
	for (const auto& TypeKeyword : TextureKeywords)
	{
		KeywordIndex = SearchingTailStr.Find(TypeKeyword, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		if (KeywordIndex != INDEX_NONE)
		{
			UE_LOG(LogTemp, Display, TEXT("Find{%s} At:%d"), *TypeKeyword,  KeywordIndex);
			TextureType=KeywordToTypeMap[TypeKeyword];
			break;
		}
	}
	if (!TextureType.IsEmpty())
	{
		NormalizedName+="_";
		NormalizedName+=TextureType;
	}
	else
	{
		NormalizedName=InTextureNames;
	}
	return NormalizedName;
}
