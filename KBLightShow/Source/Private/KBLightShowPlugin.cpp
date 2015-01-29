// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "KBLightShow.h"

#include "Core.h"
#include "Engine.h"
#include "ModuleManager.h"
#include "ModuleInterface.h"

class FKBLightShowPlugin : public IModuleInterface
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE( FKBLightShowPlugin, KBLightShow )

void FKBLightShowPlugin::StartupModule()
{
	// Supposed to not do anything for a while, just let this sink in
	LogiLedInit();

	// Make a light show actor that ticks
	new FLightShow();
}


void FKBLightShowPlugin::ShutdownModule()
{
	LogiLedShutdown();
}