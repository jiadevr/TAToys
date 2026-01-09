// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"



class FTAToysModule;
/**
 * 
 */
class TATOYS_API STexturesImporterSlate : public SCompoundWidget
{
public:
	enum class EFontType
	{
		Heading,
		Normal,
		Tint
	};
	SLATE_BEGIN_ARGS(STexturesImporterSlate)
		{
		}
	SLATE_ARGUMENT(FString, UserSelectedPath)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
protected:
	FString TargetAssetPath;
	
	//字体
	FSlateFontInfo HeadingFont;
	FSlateFontInfo NormalFont;
	FSlateFontInfo TintFont;
	//工具函数
	TSharedRef<SButton> ConstructACommonButton(const FString& TextOnButton,const FOnClicked& BindEvent);
	TSharedRef<STextBlock> ConstructATextBlock(const FString& TextOnButton,EFontType TextType=EFontType::Normal);
	
	//打开资源管理器确定路径
	FReply OnOpenBrowserClicked();
	TSharedPtr<SEditableTextBox> TargetImagePathTextBox;
	FString TargetImagePath;
	
	//刷新ListView
	FReply OnRefreshListClicked();
	TArray<FName> TextureGroupNames;
	TArray<int32> TextureCountPerGroup;
	
	//全选、全部选
	FReply OnSelectAllClicked();
	FReply OnDeselectAllClicked();
	
	//ListView
	TSharedPtr<SListView<FName>> TextureGroupListView;
	TSharedRef<SListView<FName>> ConstructTextureGroupListView();
	TSharedRef<ITableRow> ConstructSingleListViewRow(FName ItemValue,const TSharedRef<STableViewBase>& OwnerRow);
	void RefreshListView();
	void OnListViewSelectionChanged(FName ListView,ESelectInfo::Type InSelectType);
	
	//仅导入纹理或导入且创建
	FReply OnOnlyImportImageClicked();
	FReply OnImportAndCreateClicked();
	
	void OnSingleImageImported(bool bSuccess,int Index,int TotalNum,FName TextureGroupName);
	
	//进度指示
	TSharedPtr<STextBlock> ProgressText;
	
	//获取主模块执行业务逻辑
	FTAToysModule& GetMainModule() const;
};
