// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "r_defines.h"

#include "ExperimentalParams.h"


#include "ExperimentalSetup2.generated.h"

UCLASS()
class LODTESTS_API AExperimentalSetup2 : public APawn
{
	GENERATED_BODY()

  public:
	TArray<UStaticMeshComponent *> reuse_static_meshes;
	TArray<UStaticMeshComponent *> unique_static_meshes;

	TArray<ExperimentalParams> experiment_params_list;

	// May not be used because the timers are crashing the lightmaps
	FTimerHandle   lodpop_timer;
	FTimerDelegate lodpop_timer_delegate;

	AActor               *lodpop_actor  = nullptr;
	AActor               *control_actor = nullptr;
	UStaticMeshComponent *lodpop_mesh   = nullptr;
	UStaticMeshComponent *control_mesh  = nullptr;


	struct TimerInfo
	{
		float disptime                  = 2.0f; // show for 1 second
		float blanktime                 = 2.0f; // blank for 2 seconds
		bool  runtimer                  = false;
		float dt                        = 0.0f;
		bool  condition_reuse_lightmaps = false;
		bool  lodpopped_already         = false;

		void reset()
		{
			dt                = 0.0f;
			runtimer          = false;
			lodpopped_already = false;
		}
	} timerinfo;


	size_t num_ticks = 0;

	bool initialized = false;

	// Sets default values for this pawn's properties
	AExperimentalSetup2();


	// Initialization function to enumerate all desired experiment parameters
	void set_experiment_params();

	// Helper function to set model scales per model.
	ModelParams pick_model_params(ModelID model_id);

	UStaticMeshComponent* find_actor_static_mesh_component(TArray<UStaticMeshComponent*> static_mesh_components, AActor* actor);


	UFUNCTION()
	void increment_lod(UStaticMeshComponent *lod_object, const bool share_lightmap); //, TStructOnScope<FActorComponentInstanceData> &component_instance_data);


	void execute_trial(); // ExperimentalParams param);




	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;


  protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
