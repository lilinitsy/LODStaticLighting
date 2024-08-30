// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "ExperimentalParams.h"
#include "r_defines.h"

#include "Experiment1.generated.h"

UCLASS()
class LODTESTS_API AExperiment1 : public APawn
{
	GENERATED_BODY()

  public:
	TArray<UStaticMeshComponent *> reuse_static_meshes;
	TArray<UStaticMeshComponent *> unique_static_meshes;

	TArray<ExperimentalParams> experiment_params_list;

	AActor               *lodpop_actor = nullptr;
	UStaticMeshComponent *lodpop_mesh  = nullptr;

	UPROPERTY(EditAnywhere, Category = "Components")
	UCameraComponent *camera;

	UPROPERTY(EditAnywhere, Category = "Components")
	USceneComponent *camera_attachment_point;

	FString debug_msg;
	FString out_screenshot_name;
	FString out_screenshot_name_numbered;

	float model_movement_speed = 0.0f;
	float lodpop_time          = 1.0f; // when using time instead of distance


	struct TimerInfo
	{
		float  disptime                  = 2.0f; // show for 1 second
		float  blanktime                 = 2.0f; // blank for 2 seconds
		bool   runtimer                  = false;
		float  dt                        = 1.0f / 60.0f; // advance 1/60 dt at a time (so 60fps) for screenshots. If using time, change to 0.0f;
		size_t screenshot_framenum       = 0;
		bool   condition_reuse_lightmaps = false;
		bool   lodpopped_already         = false;

		void reset()
		{
			// dt                  = 0.0f;
			runtimer            = false;
			lodpopped_already   = false;
			screenshot_framenum = 0;
		}
	} timerinfo;


	size_t num_ticks = 0;

	bool initialized = false;

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool render_control = true;

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool increase_lod = true;


	// Sets default values for this pawn's properties
	AExperiment1();


	// Initialization function to enumerate all desired experiment parameters
	void set_experiment_params();

	// Helper function to set model scales per model.
	ModelParams pick_model_params(ModelID model_id);

	// Helper function to figure out the absolute value of the x value based on eccentricity and distance
	FVector compute_model_position(float distance, float eccentricity, LeftRight side);

	UStaticMeshComponent *find_actor_static_mesh_component(TArray<UStaticMeshComponent *> static_mesh_components, AActor *actor);


	UFUNCTION()
	void increment_lod(UStaticMeshComponent *lod_object, const bool share_lightmap); //, TStructOnScope<FActorComponentInstanceData> &component_instance_data);
	
	UFUNCTION()
	void decrement_lod(UStaticMeshComponent *lod_object, const bool share_lightmap); //, TStructOnScope<FActorComponentInstanceData> &component_instance_data);


	void execute_trial(); // ExperimentalParams param);




	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;


  protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
