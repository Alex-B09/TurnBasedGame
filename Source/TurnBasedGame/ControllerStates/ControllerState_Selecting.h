// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ControllerStateBase.h"
#include "ControllerState_Selecting.generated.h"

/**
 * 
 */
UCLASS()
class TURNBASEDGAME_API UControllerState_Selecting : public UControllerStateBase
{
    GENERATED_BODY()

public:
    void StartState() override;
    void OnMoveUp() override;
    void OnMoveDown() override;
    void OnMoveRight() override;
    void OnMoveLeft() override;
    void OnAction() override;
    void OnCancel() override;
};
