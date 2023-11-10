// Fill out your copyright notice in the Description page of Project Settings.


#include "LODSelector.h"

#include "LODUtils.h"
#include "r_defines.h"

// Sets default values
ALODSelector::ALODSelector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALODSelector::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALODSelector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(num_ticks % 120 == 0)
	{
		increment_lod(lod_object);
		print_lod_info(lod_object);

		if(TAKE_SCREENSHOTS)
		{
			take_screenshot(modelname, details, lod_object->GetRenderData()->CurrentFirstLODIdx);
		}
	}

	num_ticks++;
}