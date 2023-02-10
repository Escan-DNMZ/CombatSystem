// Copyright Epic Games, Inc. All Rights Reserved.

#include "DarkSoulsGameMode.h"
#include "DarkSoulsCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADarkSoulsGameMode::ADarkSoulsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
