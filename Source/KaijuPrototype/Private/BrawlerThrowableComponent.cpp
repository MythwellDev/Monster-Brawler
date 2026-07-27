#include "BrawlerThrowableComponent.h"

UBrawlerThrowableComponent::UBrawlerThrowableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBrawlerThrowableComponent::BeginPlay()
{
	Super::BeginPlay();
}