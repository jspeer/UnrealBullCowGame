// Fill out your copyright notice in the Description page of Project Settings.
#include "BullCowCartridge.h"

#if !UE_BUILD_SHIPPING
#include "HiddenWordList.h"
#else
const TArray<FString> DemoWordList;
#endif
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UBullCowCartridge::BeginPlay() // When the game starts
{
    Super::BeginPlay();

    // If we're not shipping, use the word list in HiddenWordList.h
    // Otherwise, use the wordlist located at Content/WordLists/HiddenWordList.txt
    if (!UE_BUILD_SHIPPING && false)
    {
        // Populate the valid isograms to eliminate problem words
        Isograms = GetValidWords(DemoWordList);
    }
    else
    {
        const FString WordListPath = FPaths::ProjectContentDir() / TEXT("WordLists/HiddenWordList.txt");
        FFileHelper::LoadFileToStringArrayWithPredicate(Isograms, *WordListPath, [](const FString& Word) { return Word.Len() >= 4 && Word.Len() <= 10 && IsIsogram(Word); });
    }

    // Initialize the game settings
    SetupGame();
}

void UBullCowCartridge::OnInput(const FString& PlayerInput) // When the player hits enter
{
    // If game is not over, check the player guess
    if (bGameOver)
    {
        ClearScreen();
        SetupGame();
    }
    // If game is over, clear and setup again
    else
    {
        ProcessGuess(PlayerInput);
    }
}

void UBullCowCartridge::SetupGame()
{
    HiddenWord = Isograms[FMath::RandRange(0, Isograms.Num() - 1)];
    Lives = HiddenWord.Len();
    bGameOver = false;
    PriorGuesses.Empty();

    // Welcome the player
    PrintLine(TEXT("Welcome to Bull Cows!\n"));

    // Debug output of word and len
    if (false && !UE_BUILD_SHIPPING)
    {
        PrintLine(TEXT("**DEBUG** Total Isograms loaded: %i"), Isograms.Num());
        PrintLine(TEXT("**DEBUG** Word: \"%s\" Len: %i\n"), *HiddenWord, HiddenWord.Len());
    }

    // Give the player some instructions
    PrintLine(TEXT("Guess the %i letter word!"), HiddenWord.Len());
    PrintLine(TEXT("You have %i lives."), Lives);

    // Prompt player for guess
    PrintLine(TEXT("Type in your guess and\npress enter to continue..."));
}

void UBullCowCartridge::ProcessGuess(const FString& Guess)
{
    if (Guess == HiddenWord)
    {
        PrintLine(TEXT("You have won!"));
        EndGame();
        return;
    }

    // Check to see if the guess is the correct length
    if (Guess.Len() != HiddenWord.Len())
    {
        PrintLine(TEXT("The Hidden Word is %i characters long.\nTry again."), HiddenWord.Len());
        return;
    }

    // Check to see if the guess is an isogram
    if (!IsIsogram(Guess))
    {
        PrintLine(TEXT("%s is not an isogram.\nThere are no repeating letters.\nPlease try again."), *Guess);
        return;
    }

    // Do you have enough lives to continue?
    if (--Lives > 0)
    {
        FString BullStr = TEXT("");
        FString CowsStr = TEXT("");
        FString GuessesStr = TEXT("");
        
        // Show the player bulls and cows
        FBullCowCount BullCowCount = GetBullCows(Guess);
        //*
        for (char BullChar : BullCowCount.BullChars)
        {
            BullStr += FString::Printf(TEXT("%c"), BullChar);
        }
        for (char CowsChar : BullCowCount.CowsChars)
        {
            CowsStr += FString::Printf(TEXT("%c"), CowsChar);
        }

        PriorGuesses.Emplace(Guess);
        for (FString TriedGuess : PriorGuesses)
        {
            GuessesStr += FString::Printf(TEXT("%s, "), *TriedGuess);
        }
        PrintLine(TEXT("You have %i Bulls (%s) and %i Cows (%s)."), BullCowCount.Bulls, *BullStr, BullCowCount.Cows, *CowsStr);
        PrintLine(TEXT("You have already guessed: %s"), *GuessesStr);
        /*/
        PrintLine(TEXT("You have %i Bulls and %i Cows."), BullCowCount.Bulls, BullCowCount.Cows);
        //*/
        PrintLine(TEXT("Sorry, try guessing again!\nYou have %i lives remaining."), Lives);

        return;
    }
    // You lost
    else
    {
        PrintLine(TEXT("You have no lives left.\nThe hidden word was %s.\n"), *HiddenWord);
        EndGame();
    }
}

bool UBullCowCartridge::IsIsogram(const FString& Word)
{
    // Loop through all characters making sure no characters are the same
    for (int32 Index = 0; Index < Word.Len()-1; Index++) // for each character (except the last character)
    {
        for (int32 Comparison = Index+1; Comparison < Word.Len(); Comparison++) // loop through each remaining character (not including current character)
        {
            if (Word[Index] == Word[Comparison]) // does the two character match? if so, this isn't a isogram
                return false;
        }
    }
    return true; // found no matches, this must be an isogram
}

TArray<FString> UBullCowCartridge::GetValidWords(const TArray<FString>& WordList) const
{
    // return list of filtered out words that are 4-8 chars and an isogram
    // only used when using demowordlist
    TArray<FString> ValidWords;
    for (FString CurrentWord : WordList)
    {
        if (CurrentWord.Len() >= 4 && CurrentWord.Len() <= 8 && IsIsogram(CurrentWord))
        {
            ValidWords.Emplace(CurrentWord);
        }
    }

    return ValidWords;
}

FBullCowCount UBullCowCartridge::GetBullCows(const FString& Guess) const
{
    FBullCowCount Count;

    for (int32 GuessIndex = 0; GuessIndex < Guess.Len(); GuessIndex++)
    {
        if (Guess[GuessIndex] == HiddenWord[GuessIndex])
        {
            Count.Bulls++;
            Count.BullChars.Emplace(Guess[GuessIndex]);
            continue;
        }

        for (int32 HiddenIndex = 0; HiddenIndex < HiddenWord.Len(); HiddenIndex++)
        {
            if (Guess[GuessIndex] == HiddenWord[HiddenIndex])
            {
                Count.Cows++;
                Count.CowsChars.Emplace(Guess[GuessIndex]);
                break;
            }
        }
    }

    return Count;
}

void UBullCowCartridge::EndGame()
{
    bGameOver = true;
    PrintLine(TEXT("Press enter to play again."));
}
