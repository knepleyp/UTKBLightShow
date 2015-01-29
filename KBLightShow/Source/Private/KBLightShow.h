// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "Core.h"
#include "UnrealTournament.h"

#include "LogitechLEDLib.h"

#include "KBLightShow.generated.h"

UCLASS(Blueprintable, Meta = (ChildCanTick))
class AKBLightShow : public AActor
{
	GENERATED_UCLASS_BODY()
	
};

struct KBColor
{
	int r, g, b;
	KBColor(int _r = 0, int _g = 0, int _b = 0)
	{
		r = _r;
		g = _g;
		b = _b;
	}

	bool operator==(const KBColor& Other) const
	{
		return (r == Other.r && g == Other.g && b == Other.b);
	}
};

struct FLightShow : FTickableGameObject
{
	FLightShow();
	virtual void Tick(float DeltaTime);
	virtual bool IsTickable() const { return true; }
	virtual bool IsTickableInEditor() const { return true; }

	// Put a real stat id here
	virtual TStatId GetStatId() const
	{
		return TStatId();
	}

	float DeltaTimeAccumulator;
	float FrameTimeMinimum;
	int FlashSpeed;

	bool bIsFlashingForEnd;

	KBColor CurrentBGColor;

	KBColor UTOrange;
	KBColor Black;

	void ClearColorToUTOrange();
	void ClearColorToBlack();
};