// Fill out your copyright notice in the Description page of Project Settings.


#include "TexturesImporterSlate.h"

#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "SlateOptMacros.h"
#include "TAToys.h"
#include "Kismet/KismetStringLibrary.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STexturesImporterSlate::Construct(const FArguments& InArgs)
{
	//使用传入参数初始化内容
	TargetAssetPath = InArgs._UserSelectedPath;

	//初始化字体
	HeadingFont = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	HeadingFont.Size = 20.0;
	NormalFont = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	NormalFont.Size = 15.0;
	TintFont = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	TintFont.Size = 10.0;
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		[
			ConstructATextBlock("Import To: " + TargetAssetPath, EFontType::Heading)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.0)
			[
				//需要是变量，这个值需要被填入
				SAssignNew(TargetImagePathTextBox, SEditableTextBox)
			]
			+ SHorizontalBox::Slot()
			.MaxWidth(20.0f)
			[
				ConstructACommonButton("OpenFileExplore",
				                       FOnClicked::CreateSP(this, &STexturesImporterSlate::OnOpenBrowserClicked))
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		[
			ConstructACommonButton("RefreshInfoList",
			                       FOnClicked::CreateSP(this, &STexturesImporterSlate::OnRefreshListClicked))
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				ConstructACommonButton("SelectAll",
				                       FOnClicked::CreateSP(this, &STexturesImporterSlate::OnSelectAllClicked))
			]
			+ SHorizontalBox::Slot()
			[
				ConstructACommonButton("DeselectAll",
				                       FOnClicked::CreateSP(this, &STexturesImporterSlate::OnDeselectAllClicked))
			]

		]
		+ SVerticalBox::Slot()
		.FillHeight(1.0)
		.HAlign(HAlign_Fill)
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			[
				ConstructTextureGroupListView()
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				ConstructACommonButton("ImportSelectOnly",
				                       FOnClicked::CreateSP(this, &STexturesImporterSlate::OnOnlyImportImageClicked))
			]
			+ SHorizontalBox::Slot()
			[
				ConstructACommonButton("ImportandCreateMaterial",
				                       FOnClicked::CreateSP(this, &STexturesImporterSlate::OnImportAndCreateClicked))
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		[
			SAssignNew(ProgressText, STextBlock)
		]
	];
}

TSharedRef<SButton> STexturesImporterSlate::ConstructACommonButton(const FString& InTextOnButton,
                                                                   const FOnClicked& InBindEvent)
{
	TSharedRef<SButton> Button =
		SNew(SButton)
		.ContentPadding(FMargin(5.0f))
		.OnClicked(InBindEvent)
		[
			ConstructATextBlock(InTextOnButton)
		];
	return Button;
}

TSharedRef<STextBlock> STexturesImporterSlate::ConstructATextBlock(const FString& TextOnButton,
                                                                   EFontType TextType/*=EFontType::Normal*/)
{
	FSlateFontInfo* FontInfo = nullptr;
	FLinearColor TextColor = FLinearColor::White;
	switch (TextType)
	{
	case EFontType::Heading:
		FontInfo = &HeadingFont;
		break;
	case EFontType::Normal:
		FontInfo = &NormalFont;
		break;
	case EFontType::Tint:
		FontInfo = &TintFont;
		TextColor = FLinearColor::Gray;
		break;
	}
	TSharedRef<STextBlock> TextBlock =
		SNew(STextBlock)
		.Text(FText::FromString(TextOnButton))
		.Font(*FontInfo)
		.Justification(ETextJustify::Type::Center)
		.ColorAndOpacity(TextColor);
	return TextBlock;
}

FReply STexturesImporterSlate::OnOpenBrowserClicked()
{
	FString DefaultPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	FString FileType;
	TArray<FString> OpenFileNames;
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform)
	{
		const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
		bool bCanOpen = DesktopPlatform->OpenDirectoryDialog(ParentWindowHandle,TEXT("SelectTargetDir"), *DefaultPath,
		                                                     TargetImagePath);
		if (!bCanOpen || !FPaths::DirectoryExists(TargetImagePath))
		{
			return FReply::Handled();
		}
		UE_LOG(LogTemp, Display, TEXT("Selected Folder Path:%s"), *TargetImagePath);
		TargetImagePathTextBox->SetText(FText::FromString(TargetImagePath));
	}
	return FReply::Handled();
}


FReply STexturesImporterSlate::OnRefreshListClicked()
{
	//FString不能作为ListView的模板特化参数，转成FName
	TextureGroupNames.Reset();
	TextureCountPerGroup.Reset();
	TArray<FString> TextureGroupNamesStr;
	GetMainModule().GetTextureInfos(TargetImagePath, TextureGroupNames, TextureCountPerGroup);
	RefreshListView();
	return FReply::Handled();
}

FReply STexturesImporterSlate::OnSelectAllClicked()
{
	if (TextureGroupListView.IsValid())
	{
		for (auto SingleTextureName : TextureGroupListView->GetItems())
		{
			//TextureGroupListView->SetSelection(SingleTextureName);
			TextureGroupListView->SetItemSelection(SingleTextureName, true);
		}
		TextureGroupListView->UpdateSelectionSet();
	}
	return FReply::Handled();
}

FReply STexturesImporterSlate::OnDeselectAllClicked()
{
	if (TextureGroupListView.IsValid())
	{
		TextureGroupListView->ClearSelection();
	}
	TextureGroupListView->UpdateSelectionSet();
	return FReply::Handled();
}

FReply STexturesImporterSlate::OnImportAndCreateClicked()
{
	return FReply::Handled();
}

void STexturesImporterSlate::OnSingleImageImported(bool bSuccess, int Index, int TotalNum, FName TextureGroupName)
{
	UE_LOG(LogTemp, Display, TEXT("Imported {%s} Textures, Success:%s,%d/%d"), *TextureGroupName.ToString(),
	       *UKismetStringLibrary::Conv_BoolToString(bSuccess), Index, TotalNum)
}

TSharedRef<SListView<FName>> STexturesImporterSlate::ConstructTextureGroupListView()
{
	TextureGroupListView =
		SNew(SListView<FName>)
		.ListItemsSource(&TextureGroupNames)
		.OnGenerateRow(this, &STexturesImporterSlate::ConstructSingleListViewRow)
		.SelectionMode(ESelectionMode::Type::Multi)
		.OnSelectionChanged(this, &STexturesImporterSlate::OnListViewSelectionChanged);
	return TextureGroupListView.ToSharedRef();
}

TSharedRef<ITableRow> STexturesImporterSlate::ConstructSingleListViewRow(FName ItemValue,
                                                                         const TSharedRef<STableViewBase>& OwnerRow)
{
	if (!ItemValue.IsValid())
	{
		return SNew(STableRow<FName>, OwnerRow);
	}
	FString TextureGroupNameAsString = ItemValue.ToString();
	int32 TextureGroupIndex = TextureGroupNames.Find(ItemValue);
	FString TexturesCountStr = FString::Printf(TEXT("Textures Count: %d"), TextureCountPerGroup[TextureGroupIndex]);

	TSharedRef<STableRow<FName>> ListViewRowWidget = SNew(STableRow<FName>, OwnerRow)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.0)
			[
				ConstructATextBlock(TextureGroupNameAsString, EFontType::Heading)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Right)
			[
				ConstructATextBlock(TexturesCountStr, EFontType::Tint)
			]

		];
	return ListViewRowWidget;
}

void STexturesImporterSlate::RefreshListView()
{
	if (TextureGroupListView.IsValid() && !TextureGroupNames.IsEmpty())
	{
		TextureGroupListView->RebuildList();
	}
}

void STexturesImporterSlate::OnListViewSelectionChanged(FName ListView, ESelectInfo::Type InSelectType)
{
	/*if (TextureGroupListView.IsValid())
	{
		UE_LOG(LogTemp,Display,TEXT("Selection Changed: %s"),*ListView.ToString())
		TArray<FName> Selection=TextureGroupListView->GetSelectedItems();
		for (int32 i=0;i< Selection.Num();i++)
		{
			UE_LOG(LogTemp,Display,TEXT("Selection Item: %s, Index %d/%d"),*Selection[i].ToString(),i+1,Selection.Num())
		}
	}*/
	return;
}


FReply STexturesImporterSlate::OnOnlyImportImageClicked()
{
	if (TextureGroupListView.IsValid())
	{
		TArray<FName> SelectedTextureGroupName = TextureGroupListView->GetSelectedItems();
		GetMainModule().ImportImages(TargetAssetPath, SelectedTextureGroupName,
		                             FOnImportFinished::CreateSP(this, &STexturesImporterSlate::OnSingleImageImported));
	}
	return FReply::Handled();
}

FTAToysModule& STexturesImporterSlate::GetMainModule() const
{
	return FModuleManager::Get().LoadModuleChecked<FTAToysModule>(TEXT("TAToys"));
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
