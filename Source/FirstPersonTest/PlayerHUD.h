// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "PlayerHUD.generated.h"


/**
 * 
 */
UCLASS()
class FIRSTPERSONTEST_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
    
public:

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UProgressBar* HealthBar;

    void SetHealthBar(float HealthPercent);
    
};
