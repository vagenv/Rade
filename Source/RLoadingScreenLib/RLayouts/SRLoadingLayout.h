// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "SRLayout.h"

struct FALoadingScreenSettings;
struct FLoadingLayoutSettings;

// Layout between level transition
class SRLoadingLayout : public SRLayout
{
public:
   SLATE_BEGIN_ARGS(SRLoadingLayout) {}

   SLATE_END_ARGS()

   void Construct (const FArguments &InArgs,
                   const FALoadingScreenSettings &ScreenSettings,
                   const FLoadingLayoutSettings  &LayoutSettings);
};

