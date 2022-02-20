// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Console/Cartridge.h"
#include "BullCowCartridge.generated.h"

struct FBullCowCount
{
	int32 Bulls = 0;
	int32 Cows = 0;
	TArray<char> BullChars;
	TArray<char> CowsChars;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BULLCOWGAME_API UBullCowCartridge : public UCartridge
{
	GENERATED_BODY()

	public:
	virtual void BeginPlay() override;
	virtual void OnInput(const FString& PlayerInput) override;
	static bool IsIsogram(const FString& Word);
	TArray<FString> GetValidWords(const TArray<FString>& WordList) const;

	// Your declarations go below!
	private:
	void SetupGame();
	void ProcessGuess(const FString& Guess);
	FBullCowCount GetBullCows(const FString& Guess) const;
	void EndGame();

	FString HiddenWord;
	int32 Lives;
	bool bGameOver;
	TArray<FString> Isograms;
	TArray<FString> PriorGuesses;
};
