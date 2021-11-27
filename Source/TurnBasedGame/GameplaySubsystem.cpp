// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplaySubsystem.h"
#include <Runtime/AIModule/Classes/AIController.h>
#include <Runtime/AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h>

void UGameplaySubsystem::SetupGrid(AGameGrid* grid)
{
    if (grid == nullptr)
    {
        UE_LOG(LogTemp, Log, TEXT("UGameplaySubsystem::SetupGrid - Invalid grid"));
        return;
    }

    mGrid = grid;
}

void UGameplaySubsystem::AddCharacter(TSubclassOf<AGameCharacter> characterClass, AGridTile* tile, bool isPlayerControllable)
{
    // reject if already occupied
    if (auto info = GetGridInfo(tile))
    {
        UE_LOG(LogTemp, Log, TEXT("UGamplaySubsystem::AddCharacter - tile already occupied -- entry rejected"));
        return;
    }

    // add function to get data

    auto transform = tile->GetCharacterPosition();
    auto characterActor = Cast<AGameCharacter>(GetWorld()->SpawnActor(characterClass, &transform, FActorSpawnParameters()));
    characterActor->SpawnDefaultController(); // TODO - test if i still need this
    auto info = NewObject<UCharacterGridInfo>();

    if (characterActor == nullptr || info == nullptr)
    {
        UE_LOG(LogTemp, Log, TEXT("UGamplaySubsystem::AddCharacter - invalid object -- need debug"));
        return;
    }

    info->mCharacter = characterActor;
    info->mTile = tile;
    info->mIsPlayerCharacter = isPlayerControllable;

    mCharacters.Add(info);
}

ETileOccupationState UGameplaySubsystem::GetTileOccupationStatus(AGridTile* tile) const
{
    if (!tile)
    {
        UE_LOG(LogTemp, Log, TEXT("UGamplaySubsystem::GetTileStatus - invalid tile"));
        return ETileOccupationState::Empty; // TODO - add error?
    }

    if (auto info = GetGridInfo(tile))
    {
        if (info->mIsPlayerCharacter)
        {
            return ETileOccupationState::IsCharacterPlayer;
        }
        return ETileOccupationState::IsCharacterEnemy;
    }

    return ETileOccupationState::Empty;
}

UCharacterGridInfo* UGameplaySubsystem::GetGridInfo(AGridTile* tile) const
{
    if (tile == nullptr)
    {
        UE_LOG(LogTemp, Log, TEXT("UGamplaySubsystem::GetGridInfo - invalid tile"));
        return nullptr;
    }

    for (auto character : mCharacters)
    {
        if (character->mTile == tile)
        {
            return character;
        }
    }

    return nullptr;
}

UCharacterGridInfo* UGameplaySubsystem::GetGridInfo(AGameCharacter* character) const
{
    if (character == nullptr)
    {
        UE_LOG(LogTemp, Log, TEXT("UGamplaySubsystem::GetGridInfo - invalid character"));
        return nullptr;
    }

    for (auto info : mCharacters)
    {
        if (info->mCharacter == character)
        {
            return info;
        }
    }

    return nullptr;
}

void UGameplaySubsystem::MoveCharacter(AGameCharacter* character, AGridTile* tileToMoveTo)
{
    UE_LOG(LogTemp, Log, TEXT("UGamplaySubsystem::MoveCharacter - moving character"));

    // verify if null
    if (character == nullptr || tileToMoveTo == nullptr)
    {
        UE_LOG(LogTemp, Log, TEXT("UGamplaySubsystem::MoveCharacter - invalid input"));
        return;
    }


    // get info
    if (auto info = GetGridInfo(character))
    {
        TArray<AGridTile*> positions; // TODO - get the path (tile by tile) to the destination
        positions.Add(tileToMoveTo);

        UE_LOG(LogTemp, Log, TEXT("UGamplaySubsystem::MoveCharacter - hum...."));
        info->mCharacter->Move(info->mTile, positions);
        UE_LOG(LogTemp, Log, TEXT("UGamplaySubsystem::MoveCharacter - test"));
        info->mTile = tileToMoveTo;
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("UGamplaySubsystem::MoveCharacter - character not found"));
    }
}

void UGameplaySubsystem::TeleportCharacter(AGameCharacter* character, AGridTile* tileToTeleportTo)
{
    UE_LOG(LogTemp, Log, TEXT("UGamplaySubsystem::TeleportCharacter - teleporting character"));

    // verify if null
    if (character == nullptr || tileToTeleportTo == nullptr)
    {
        UE_LOG(LogTemp, Log, TEXT("UGamplaySubsystem::TeleportCharacter - invalid input"));
        return;
    }

    // get info
    if (auto info = GetGridInfo(character))
    {
        if (info->mTile != tileToTeleportTo)
        {
            info->mCharacter->Teleport(tileToTeleportTo);
            info->mTile = tileToTeleportTo;
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("UGamplaySubsystem::TeleportCharacter - character not found"));
    }
}

AGameCharacter* UGameplaySubsystem::GetCharacter(AGridTile* tile) const
{
    if (tile == nullptr)
    {
        UE_LOG(LogTemp, Log, TEXT("UGamplaySubsystem::GetCharacter - invalid tile"));
    }

    for (auto info : mCharacters)
    {
        if (info->mTile == tile)
        {
            return info->mCharacter;
        }
    }

    UE_LOG(LogTemp, Log, TEXT("UGamplaySubsystem::GetCharacter - not found character"));
    return nullptr;
}

AGridTile* UGameplaySubsystem::GetTile(AGameCharacter* character) const
{
    if (!character)
    {
        UE_LOG(LogTemp, Log, TEXT("UGamplaySubsystem::GetTile - invalid input"));
        return nullptr;
    }

    for (auto info : mCharacters)
    {
        if (info->mCharacter == character)
        {
            return info->mTile;
        }
    }
    return nullptr;
}


void UGameplaySubsystem::HighlighGridForMovement(AGameCharacter* character) const
{
    // get the tiles
    auto tiles = mGrid->GetTiles(GetTile(character), 3);

    for (auto tile : tiles)
    {
        tile->SetToMovement();
    }
}

void UGameplaySubsystem::HighlighGridForAttack(AGameCharacter* character) const
{
    // get the tiles
    auto tiles = mGrid->GetTiles(GetTile(character), 1);

    for (auto tile : tiles)
    {
        // TODO - add check to see if there is a character?
        tile->SetToAttack();
    }
}


void UGameplaySubsystem::HideGridHighlight() const
{
    mGrid->HideSelectors(); // TODO - should move to subsystem?
}

TArray<AGridTile*> UGameplaySubsystem::GetAvailableMovementTiles(AGameCharacter* character)
{
    // TODO this need to be used in HighlighGridForCharacter
    auto tile = GetTile(character);
    auto availableTiles = mGrid->GetTiles(tile, 3); // replace the 3 to use the abilitySet
    return availableTiles;
}

TArray<AGridTile*> UGameplaySubsystem::GetAvailableAttackTiles(AGameCharacter* character)
{
    // TODO this need to be used in HighlighGridForAttack
    auto tile = GetTile(character);
    auto availableTiles = mGrid->GetTiles(tile, 1); // replace the 1 to use the abilitySet -- if ever used
    return availableTiles;
}


AGameCharacter* UGameplaySubsystem::GetEnemyCharacter()
{
    for (auto character : mCharacters)
    {
        if (!character->mIsPlayerCharacter)
        {
            return character->mCharacter;
        }
    }
    return nullptr;
}
