// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ABaseItem::ABaseItem()
{
 
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComp->SetupAttachment(SceneComponent);
	CollisionComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComp->SetupAttachment(CollisionComp);

	// 이벤트 바인딩
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnItemOverlap);
	CollisionComp->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::OnItemEndOverlap);
}

void ABaseItem::OnItemOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		ActivateItem(OtherActor);
	}
}

void ABaseItem::OnItemEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtehrActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
}

void ABaseItem::ActivateItem(AActor* Activator)
{
	UParticleSystemComponent* Particle = nullptr;
	if (Activator)
	{
		if (PickupParticle)
		{
			Particle = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				PickupParticle,
				GetActorLocation(),
				GetActorRotation(),
				false
			);
		}

		if (PickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				GetWorld(),
				PickupSound,
				GetActorLocation()
			);
		}

	}
	
	if (Particle)
	{
		FTimerHandle DestroyParticleTimerHandle;
		TWeakObjectPtr<UParticleSystemComponent> WeakParticle = Particle;

		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[WeakParticle]()
			{
				if (WeakParticle.IsValid())
				{
					WeakParticle->DestroyComponent();
				}
			},
			2.0f,
			false
		);
		Particle = nullptr;
	}
}

FName ABaseItem::GetItemType() const
{
	return ItemType;
}

void ABaseItem::DestroyItem()
{
	Destroy();
}



