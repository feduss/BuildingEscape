// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	TargetActorRotatorClose = GetOwner()->GetActorRotation();
	TargetActorRotatorOpen = FRotator(TargetActorRotatorClose.Pitch, TargetActorRotatorClose.Yaw + OpenAngle, TargetActorRotatorClose.Roll); //Y, Z, X
	ActorThatOpensTheDoor = GetWorld()->GetFirstPlayerController()->GetPawn();
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Apro la porta quando il player preme la pedana a pressione
	if (ActorThatOpensTheDoor != nullptr && PressurePlate->IsOverlappingActor(ActorThatOpensTheDoor)) {
		MoveDoor(DeltaTime, TargetActorRotatorOpen);
		//Salvo l'ultima volta che ho premuto la pedana
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	//Chiudo la porta quando il player non sta premendo la pedana a pressione
	//Ma solo dopo due secondi dall'ultime pressione
	else if(GetWorld()->GetTimeSeconds() - DoorLastOpened > DoorClosingDelay) {
		MoveDoor(DeltaTime, TargetActorRotatorClose);
	}

	
}

void UOpenDoor::MoveDoor(float DeltaTime, FRotator TargetRotator) {
	FRotator CurrentActorRotator = GetOwner()->GetActorRotation();
	//Lerp con Deltatime permette di interpolare in maniera indipendente dai frame
	//FInterpConstantTo interpola con step costante (sempre gli stessi gradi al secondo)
	//FInterpTo interpola come Lerp, ma sempre sfruttando il Deltatime
	//Deltatime è il tempo che unreal ci mette a calcolare un frame
	//L'ultimo parametro indica i gradi al secondo
	CurrentActorRotator.Yaw = FMath::Lerp(CurrentActorRotator.Yaw, TargetRotator.Yaw, DeltaTime);
	//CurrentActorRotator.Yaw = FMath::FInterpConstantTo(CurrentActorRotator.Yaw, TargetActorRotator.Yaw, DeltaTime, 0.05f);
	//CurrentActorRotator.Yaw = FMath::FInterpTo(CurrentActorRotator.Yaw, TargetActorRotator.Yaw, DeltaTime, 0.05f);
	GetOwner()->SetActorRotation(CurrentActorRotator);
}

