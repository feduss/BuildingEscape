// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/HUD.h"
#define out

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();

	SetupInputComponent();
	
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	PrintHitObjectToHUD(GetHitObject());

	if (PhysicsHandle->GetGrabbedComponent() != nullptr && PhysicsHandle->GetGrabbedComponent()->IsSimulatingPhysics()) {
		//UE_LOG(LogTemp, Warning, TEXT("%s is moving!"), *PhysicsHandle->GetGrabbedComponent()->GetName());
		GetLineTraceEnd();
		PhysicsHandle->SetTargetLocationAndRotation(LineTraceEnd, FRotator(0.f,0.f,0.f));
		//UE_LOG(LogTemp, Warning, TEXT("Moving %s to %s"), *PhysicsHandle->GetGrabbedComponent()->GetName(), *LineTraceEnd.ToString());
	}
	else {
		//UE_LOG(LogTemp, Warning, TEXT("No Object grabbed."));
	}
}


void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("%s has null InputComponent"), *GetOwner()->GetName());
	}
	else {
		//Il primo parametro è lo stesso nome dell'azione definita in Project Settings/Engine/Input
		InputComponent->BindAction("Grab", EInputEvent::IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", EInputEvent::IE_Released, this, &UGrabber::Release);
	}
}

void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	if (PhysicsHandle == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("%s has null PhysicsHandleComponent"), *GetOwner()->GetName());
	}
	else {

	}
}

FHitResult UGrabber::GetHitObject()
{
	FHitResult HitResult;
	//Get the player viewport
	GetLineTraceEnd();

	//Trace raycast
	///Filtro il raycast ignorando l'owner di questo script
	FCollisionQueryParams CollisionQueryParams = FCollisionQueryParams(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByChannel(
		out HitResult,
		LineTraceStart,
		LineTraceEnd,
		ECollisionChannel::ECC_PhysicsBody,
		CollisionQueryParams
	);
	return HitResult;
}

void UGrabber::GetLineTraceEnd()
{
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		out PlayerViewPointLocation,
		out PlayerViewPointRotation
	);

	//Log these values
	/*UE_LOG(LogTemp, Warning, TEXT("PLocation: %s, PRotation: %s"),
	*PlayerViewPointLocation.ToString(),
	*PlayerViewPointRotation.ToString()
	);*/
	LineTraceStart = PlayerViewPointLocation;
	LineTraceEnd = PlayerViewPointLocation + (PlayerViewPointRotation.Vector() * Distance);
}

//TODO
void UGrabber::PrintHitObjectToHUD(FHitResult HitResult)
{
	if (HitResult.GetActor() != nullptr) {
		//Log physics body hit
		//UE_LOG(LogTemp, Warning, TEXT("Premi \"F\" per raccogliere %s"), *HitResult.GetActor()->GetActorLabel());
		/*FString Text = FString::Printf(TEXT("Premi \"F\" per raccogliere %s"), *HitResult.GetActor()->GetActorLabel());
		GetWorld()->GetFirstPlayerController()->GetHUD()->DrawText(
			Text,
			FColor::White,
			100.f,
			100.f,
			(UFont *)14
		);*/
	}
}

void UGrabber::Grab() {
	//UE_LOG(LogTemp, Warning, TEXT("Grab pressed"));
	GetLineTraceEnd();
	LastObjectTaken = GetHitObject();
	if (LastObjectTaken.GetActor() != nullptr) {
		PhysicsHandle->GrabComponentAtLocation(
			LastObjectTaken.GetComponent(),
			NAME_None,
			LineTraceEnd
		);
		UE_LOG(LogTemp, Warning, TEXT("Object grabbed: %s"), *LastObjectTaken.GetActor()->GetActorLabel());
	}
}
	
void UGrabber::Release() {
	//UE_LOG(LogTemp, Warning, TEXT("Grab release"));
	if (LastObjectTaken.GetActor() != nullptr) {
		PhysicsHandle->ReleaseComponent();
		UE_LOG(LogTemp, Warning, TEXT("Object released: %s"), *LastObjectTaken.GetActor()->GetActorLabel());
	}
}

