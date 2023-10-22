// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "SLoadingWidget.h"

class SVerticalLoadingWidget : public SLoadingWidget
{
public:
   SLATE_BEGIN_ARGS(SVerticalLoadingWidget) {}

   SLATE_END_ARGS()

   void Construct(const FArguments& InArgs, const FLoadingWidgetSettings& Settings);
};
