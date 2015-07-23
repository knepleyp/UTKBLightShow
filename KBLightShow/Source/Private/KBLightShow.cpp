// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "KBLightShow.h"
#include "UnrealTournament.h"
#include "UTPlayerController.h"
#include "UTGameState.h"

DEFINE_LOG_CATEGORY_STATIC(LogUTKBLightShow, Log, All);

AKBLightShow::AKBLightShow(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FLightShow::FLightShow()
{
	FrameTimeMinimum = 0.03f;
	DeltaTimeAccumulator = 0;
	bIsFlashingForEnd = false;
	bInitialized = false;

	UTOrange.r = 100;
	UTOrange.g = 29;
	UTOrange.b = 1;

	Black.r = 0;
	Black.g = 0;
	Black.b = 0;

	CurrentBGColor.r = 100;
	CurrentBGColor.g = 100;
	CurrentBGColor.b = 100;

	FlashSpeed = 100;
}

IMPLEMENT_MODULE(FLightShow, KBLightShow)

void FLightShow::StartupModule()
{
	bInitialized = LogiLedInit();
}


void FLightShow::ShutdownModule()
{
	LogiLedShutdown();
}

void FLightShow::Tick(float DeltaTime)
{
//	return;

	// We may be going 120hz, don't spam the device
	DeltaTimeAccumulator += DeltaTime;
	if (DeltaTimeAccumulator < FrameTimeMinimum)
	{
		return;
	}
	DeltaTimeAccumulator = 0;

	if (!bInitialized)
	{
		// Supposed to not do anything for a while, just let this sink in
		bInitialized = LogiLedInit();
		DeltaTimeAccumulator = -10 * FrameTimeMinimum;
		return;
	}

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
				
				bool bSetLightingResult = false;

				bSetLightingResult = LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::W, 0, 0, 100);
				bSetLightingResult = LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::A, 0, 0, 100);
				bSetLightingResult = LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::S, 0, 0, 100);
				bSetLightingResult = LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::D, 0, 0, 100);
				bSetLightingResult = LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::SPACE, 0, 0, 100);

				if (UTPC->GetUTCharacter() && !UTPC->GetUTCharacter()->IsDead())
				{
					LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::J, 64, 0, 64);
					LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::K, 64, 0, 64);
					LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::L, 64, 0, 64);

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
						WeaponGroupColor = KBColor(0, 0, 100);

						for (TInventoryIterator<AUTWeapon> It(UTPC->GetUTCharacter()); It; ++It)
						{
							AUTWeapon* Weap = *It;
							if (Weap->HasAnyAmmo())
							{
								if (Weap->ClassicGroup == (i + 1))
								{
									bFoundWeapon = true;
									WeaponGroupColor.r = Weap->IconColor.R * 100;
									WeaponGroupColor.g = Weap->IconColor.G * 100;
									WeaponGroupColor.b = Weap->IconColor.B * 100;
									break;
								}
							}
						}

						if ((i + 1) == CurrentWeaponGroup)
						{
							LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName(LogiLed::KeyName::ONE + i), 100, 0, 0);
						}
						else if (bFoundWeapon)
						{
							// WeaponColor stored in Weap->IconColor isn't specific enough
							LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName(LogiLed::KeyName::ONE + i), 0, 100, 0);
							//LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName(LogiLed::KeyName::ONE + i), WeaponGroupColor.r, WeaponGroupColor.g, WeaponGroupColor.b);
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

	if (LogiLedSetLighting(UTOrange.r, UTOrange.g, UTOrange.b))
	{
		CurrentBGColor = UTOrange;
	}
	else
	{
		UE_LOG(LogUTKBLightShow, Warning, TEXT("Failed to set lighting"));
	}
}

void FLightShow::ClearColorToBlack()
{
	if (CurrentBGColor == Black)
	{
		return;
	}

	if (LogiLedSetLighting(Black.r, Black.g, Black.b))
	{
		CurrentBGColor = Black;
	}
	else
	{
		UE_LOG(LogUTKBLightShow, Warning, TEXT("Failed to set lighting"));
	}
}