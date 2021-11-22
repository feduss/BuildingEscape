// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Components/PrimitiveComponent.h"
#define OUT

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
	//ActorThatOpensTheDoor = GetWorld()->GetFirstPlayerController()->GetPawn();
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Apro la porta quando il player preme la pedana a pressione (UPDATE: rimpiazzato da TotalMassOfActors)
	//if (ActorThatOpensTheDoor != nullptr && PressurePlate->IsOverlappingActor(ActorThatOpensTheDoor)) {
	if(TotalMassOfActors() > MassToPress){
		//UE_LOG(LogTemp, Warning, TEXT("%s is pressing the pressure plate!"), *ActorThatOpensTheDoor->GetActorLabel());
		MoveDoor(DeltaTime, TargetActorRotatorOpen, DoorOpeningSpeed);
		//Salvo l'ultima volta che ho premuto la pedana
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	//Chiudo la porta quando il player non sta premendo la pedana a pressione
	//Ma solo dopo due secondi dall'ultime pressione
	else if(GetWorld()->GetTimeSeconds() - DoorLastOpened > DoorClosingDelay) {
		MoveDoor(DeltaTime, TargetActorRotatorClose, DoorClosingSpeed);
	}	
}

void UOpenDoor::MoveDoor(float DeltaTime, FRotator TargetRotator, float Speed) {
	FRotator CurrentActorRotator = GetOwner()->GetActorRotation();
	//Lerp con Deltatime permette di interpolare in maniera indipendente dai frame
	//FInterpConstantTo interpola con step costante (sempre gli stessi gradi al secondo)
	//FInterpTo interpola come Lerp, ma sempre sfruttando il Deltatime
	//Deltatime è il tempo che unreal ci mette a calcolare un frame
	//L'ultimo parametro indica i gradi al secondo
	CurrentActorRotator.Yaw = FMath::Lerp(CurrentActorRotator.Yaw, TargetRotator.Yaw, DeltaTime * Speed);
	//CurrentActorRotator.Yaw = FMath::FInterpConstantTo(CurrentActorRotator.Yaw, TargetActorRotator.Yaw, DeltaTime, 0.05f);
	//CurrentActorRotator.Yaw = FMath::FInterpTo(CurrentActorRotator.Yaw, TargetActorRotator.Yaw, DeltaTime, 0.05f);
	GetOwner()->SetActorRotation(CurrentActorRotator);
}

float UOpenDoor::TotalMassOfActors() {
	float TotalMass = 0.f;

	TArray<AActor*> OverlappingActors;

	PressurePlate->GetOverlappingActors(OUT OverlappingActors, nullptr);

	for (AActor* OverlappingActor : OverlappingActors) {
		
		UPrimitiveComponent* PrimitiveComponent = OverlappingActor->FindComponentByClass<UPrimitiveComponent>();
		if (PrimitiveComponent != nullptr) {
			TotalMass += PrimitiveComponent->GetMass();
		}
	}



	return TotalMass;
}

