// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "KBLightShow.h"
#include "UnrealTournament.h"
#include "UTPlayerController.h"
#include "UTGameState.h"

AKBLightShow::AKBLightShow(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FLightShow::FLightShow()
{
	FrameTimeMinimum = 0.03f;
	DeltaTimeAccumulator = 0;
	bIsFlashingForEnd = false;
	
	UTOrange.r = 255;
	UTOrange.g = 73;
	UTOrange.b = 2;

	Black.r = 0;
	Black.g = 0;
	Black.b = 0;

	CurrentBGColor.r = 255;
	CurrentBGColor.g = 255;
	CurrentBGColor.b = 255;

	FlashSpeed = 100;
}

void FLightShow::Tick(float DeltaTime)
{
	// We may be going 120hz, don't spam the device
	DeltaTimeAccumulator += DeltaTime;
	if (DeltaTimeAccumulator < FrameTimeMinimum)
	{
		return;
	}
	DeltaTimeAccumulator = 0;

	bool bShouldFlashForEnd = false;

	AUTPlayerController* UTPC = Cast<AUTPlayerController>(GEngine->GetFirstLocalPlayerController(GWorld));
	AUTGameState* GS = GWorld->GetGameState<AUTGameState>();
	if (UTPC)
	{
		if (GS)
		{
			if (GS->HasMatchEnded())
			{
				ClearColorToUTOrange();

				bShouldFlashForEnd = true;

				if (!bIsFlashingForEnd)
				{
					if (GS->WinningTeam != nullptr)
					{
						bIsFlashingForEnd = true;
						if (GS->WinningTeam->GetTeamNum() == UTPC->GetTeamNum())
						{
							LogiLedFlashLighting(0, 0, 100, LOGI_LED_DURATION_INFINITE, FlashSpeed);
						}
						else
						{
							LogiLedFlashLighting(100, 0, 0, LOGI_LED_DURATION_INFINITE, FlashSpeed);
						}
					}
					else if (GS->WinnerPlayerState != nullptr)
					{
						bIsFlashingForEnd = true;
						if (GS->WinnerPlayerState == UTPC->PlayerState)
						{
							LogiLedFlashLighting(0, 0, 100, LOGI_LED_DURATION_INFINITE, FlashSpeed);
						}
						else
						{
							LogiLedFlashLighting(100, 0, 0, LOGI_LED_DURATION_INFINITE, FlashSpeed);
						}
					}
				}
			}
			else if (GS->HasMatchStarted())
			{
				ClearColorToBlack();

				LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::W, 0, 0, 255);
				LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::A, 0, 0, 255);
				LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::S, 0, 0, 255);
				LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::D, 0, 0, 255);
				LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::SPACE, 0, 0, 255);

				if (UTPC->GetUTCharacter() && !UTPC->GetUTCharacter()->IsDead())
				{
					LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::J, 165, 0, 165);
					LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::K, 165, 0, 165);
					LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::L, 165, 0, 165);

					int32 CurrentWeaponGroup = -1;
					if (UTPC->GetUTCharacter()->GetWeapon())
					{
						CurrentWeaponGroup = UTPC->GetUTCharacter()->GetWeapon()->ClassicGroup;
					}

					bool bFoundWeapon = false;
					KBColor WeaponGroupColor;
					for (int32 i = 0; i < 10; i++)
					{
						bFoundWeapon = false;
						WeaponGroupColor = KBColor(0, 0, 255);

						for (TInventoryIterator<AUTWeapon> It(UTPC->GetUTCharacter()); It; ++It)
						{
							AUTWeapon* Weap = *It;
							if (Weap->HasAnyAmmo())
							{
								if (Weap->ClassicGroup == (i + 1))
								{
									bFoundWeapon = true;
									WeaponGroupColor.r = Weap->IconColor.R * 255;
									WeaponGroupColor.g = Weap->IconColor.G * 255;
									WeaponGroupColor.b = Weap->IconColor.B * 255;
									break;
								}
							}
						}

						if ((i + 1) == CurrentWeaponGroup)
						{
							LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName(LogiLed::KeyName::ONE + i), 255, 0, 0);
						}
						else if (bFoundWeapon)
						{
							// WeaponColor stored in Weap->IconColor isn't specific enough
							LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName(LogiLed::KeyName::ONE + i), 0, 255, 0);
						}
						else
						{
							LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName(LogiLed::KeyName::ONE + i), 0, 0, 0);
						}
					}
				}
				else
				{
					LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::J, 0, 0, 0);
					LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::K, 0, 0, 0);
					LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::L, 0, 0, 0);

					for (int32 i = 0; i < 10; i++)
					{
						LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName(LogiLed::KeyName::ONE + i), 0, 0, 0);
					}
				}
			}
			else
			{
				ClearColorToUTOrange();
			}
		}
		else
		{
			ClearColorToUTOrange();
		}
	}
	else
	{
		ClearColorToUTOrange();
	}

	if (!bShouldFlashForEnd && bIsFlashingForEnd)
	{
		bIsFlashingForEnd = false;
		LogiLedStopEffects();
		ClearColorToBlack();
	}
}

void FLightShow::ClearColorToUTOrange()
{
	if (CurrentBGColor == UTOrange)
	{
		return;
	}

	CurrentBGColor = UTOrange;
	LogiLedSetLighting(UTOrange.r, UTOrange.g, UTOrange.b);
}

void FLightShow::ClearColorToBlack()
{
	if (CurrentBGColor == Black)
	{
		return;
	}

	CurrentBGColor = Black;
	LogiLedSetLighting(Black.r, Black.g, Black.b);
}
