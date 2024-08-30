// Fill out your copyright notice in the Description page of Project Settings.


#include "Experiment1.h"
#include "Engine/MapBuildDataRegistry.h"
#include "LODUtils.h"
#include "StaticMeshComponentLODInfo.h"

#include "Lightmap.h"


// Sets default values
AExperiment1::AExperiment1()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// camera setup
	camera_attachment_point = CreateDefaultSubobject<USceneComponent>(TEXT("camera_attachment_point"));
	camera_attachment_point->SetupAttachment(RootComponent);
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("camera"));
	camera->SetupAttachment(camera_attachment_point);

	set_experiment_params();
}

// Called when the game starts or when spawned
void AExperiment1::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AExperiment1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if(num_ticks == 600)
	{
		reuse_static_meshes  = get_static_mesh_components(GetWorld());
		unique_static_meshes = get_static_mesh_components_unique_lightmaps(GetWorld());

		initialized = true;

		UE_LOG(LogTemp, Log, TEXT("Scene setup initalized\n"));
		UE_LOG(LogTemp, Log, TEXT("Num conditions: %d\n"), experiment_params_list.Num());
	}

	/*if(reuse_static_meshes.Num() > 0 && num_ticks > 300 && num_ticks % 120 == 0)
	{
	for(UStaticMeshComponent* static_mesh_component : reuse_static_meshes)
	{
	increment_lod(static_mesh_component, true);
	}

	for(UStaticMeshComponent* static_mesh_component : unique_static_meshes)
	{
	increment_lod(static_mesh_component, false);
	}
	}*/

	// First: Make it pop 2s after hitting spacebar.
	if(timerinfo.runtimer)
	{
		// timerinfo.dt += DeltaTime;
		// FVector lodpop_position = lodpop_actor->GetActorLocation();
		// UE_LOG(LogTemp, Log, TEXT("Actor location: %f %f %f\n"), lodpop_position.X, lodpop_position.Y, lodpop_position.Z);
		FVector eye_location     = camera_attachment_point->GetComponentLocation();
		float   distance_to_move = model_movement_speed * timerinfo.dt;

		eye_location.X += distance_to_move * (increase_lod ? -1.0f : 1.0f);
		UE_LOG(LogTemp, Log, TEXT("eye_location.X: %f\n"), eye_location.X);

		//eye_location.X -= distance_to_move;
		camera_attachment_point->SetWorldLocation(eye_location);

		if(increase_lod && !render_control && timerinfo.screenshot_framenum >= lodpop_time * DESIRED_FRAMERATE && !timerinfo.lodpopped_already)
		{
			increment_lod(lodpop_mesh, timerinfo.condition_reuse_lightmaps);
			timerinfo.lodpopped_already = true;
		}

		else if(!increase_lod && !render_control && timerinfo.screenshot_framenum >= lodpop_time * DESIRED_FRAMERATE && !timerinfo.lodpopped_already)
		{
			decrement_lod(lodpop_mesh, timerinfo.condition_reuse_lightmaps);
			timerinfo.lodpopped_already = true;
		}

		// out_screenshot_name += "_" + FString::FromInt(timerinfo.screenshot_framenum);
		out_screenshot_name_numbered = out_screenshot_name + FString::FromInt(timerinfo.screenshot_framenum);
		const FString ImageDirectory = FString::Printf(TEXT("%s/Screenshots/%s/"), *FPaths::ProjectDir(), *out_screenshot_name);
		FScreenshotRequest::RequestScreenshot(ImageDirectory + FString::FromInt(timerinfo.screenshot_framenum), false, false);

		timerinfo.screenshot_framenum++;

		// if(timerinfo.dt >= timerinfo.disptime)
		if(timerinfo.screenshot_framenum >= timerinfo.disptime * DESIRED_FRAMERATE)
		{
			timerinfo.reset();
			out_screenshot_name = "";
			debug_msg           = "";

			if(experiment_params_list.Num() > 0)
			{
				execute_trial();
			}

			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("All trials done!"));
			}
		}
	}


	num_ticks++;
}


// Called to bind functionality to input
void AExperiment1::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("ExecuteTrial", IE_Released, this, &AExperiment1::execute_trial);
}


UStaticMeshComponent *AExperiment1::find_actor_static_mesh_component(TArray<UStaticMeshComponent *> static_mesh_components, AActor *actor)
{
	for(UStaticMeshComponent *smc : static_mesh_components)
	{
		if(smc->GetAttachmentRootActor() == actor)
		{
			UE_LOG(LogTemp, Log, TEXT("Found root actor, it's %s\n"), *smc->GetAttachmentRootActor()->GetActorLabel(false));
			return smc;
		}
	}


	return nullptr;
}


void AExperiment1::execute_trial() // ExperimentalParams param)
{
	if(initialized && !timerinfo.runtimer)
	{
		UE_LOG(LogTemp, Log, TEXT("execute_trial called\n"));
		size_t num_conditions_left = experiment_params_list.Num();
		UE_LOG(LogTemp, Log, TEXT("%d conditions left\n"), num_conditions_left);

		if(num_conditions_left == 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("All trials done!"));
			return;
		}

		size_t             condition_num     = FMath::RandRange(0, (int32_t) num_conditions_left - 1);
		size_t             show_lodpop_first = FMath::RandRange(0, 1);
		ExperimentalParams condition         = experiment_params_list[condition_num];
		ModelParams        params            = condition.model_id;
		experiment_params_list.RemoveAt(condition_num);

		FString methodname      = condition.method == UNIQUE ? "UNIQUE" : "REUSE";
		bool    reuse_lightmaps = condition.method == UNIQUE ? false : true;
		model_movement_speed    = condition.model_movement_speed;
		FString basemodelname;


		switch(condition.model_id.id)
		{
			case CLAYPOT:
				basemodelname = "Claypot";
				break;
			case WOOD_LOGS:
				basemodelname = "WoodLogs";
				break;
			case CASTLE_STRUCTURE:
				basemodelname = "CastleStructure";
				break;
			case CLAYJUG:
				basemodelname = "Clayjug";
				break;
			case ICELANDIC_SPIRE:
				basemodelname = "IcelandicSpire";
				break;
			case VOLCANIC_ROCK:
				basemodelname = "VolcanicRock";
				break;
			case PINE_CONE:
				basemodelname = "PineCone";
				break;
			case WOOD_STUMP:
				basemodelname = "WoodStump";
				break;
			default: // something went wrong
				basemodelname = "NOT_FOUND";
				break;
		}

		lodpop_actor = nullptr;
		lodpop_mesh  = nullptr;


		for(AActor *actor : GetWorld()->GetLevel(0)->Actors)
		{
			if(actor)
			{
				FString actor_name = actor->GetActorLabel(false);

				// CONTROL check is just so we don't have to remove the models from the scenes to run this.
				if(!actor_name.Contains("CONTROL") && actor_name.Contains(basemodelname) && actor_name.Contains(methodname))
				{

					lodpop_actor = actor;
				}

				else
				{
					if(actor->IsRootComponentMovable())
					{
						actor->SetActorLocation(FVector(5000.0f, 0.0f, 44.0f));
						UE_LOG(LogTemp, Log, TEXT("%s was moved\n"), *actor_name);
					}
				}
			}
		}

		if(lodpop_actor != nullptr)
		{
			debug_msg += basemodelname + " " + methodname + " ";
			out_screenshot_name += basemodelname + "_" + methodname;


			TArray<UStaticMeshComponent *> static_mesh_list = condition.method == UNIQUE ? unique_static_meshes : reuse_static_meshes;
			lodpop_mesh                                     = find_actor_static_mesh_component(static_mesh_list, lodpop_actor);

			// Set appropriate lodlvl
			lodpop_mesh->SetForcedLodModel(condition.lodlvl + 1);

			// lodpop_actor->SetActorLocation(compute_model_position(condition.distance, condition.eccentricity, condition.leftright));
			//  Instead, try moving the eye
			FVector model_position          = lodpop_actor->GetActorLocation();
			FVector eye_location            = camera_attachment_point->GetComponentLocation();
			float   starting_eye_position_x = model_position.X - condition.distance;
			camera_attachment_point->SetWorldLocation(FVector(starting_eye_position_x, eye_location.Y, 64.0f));


			if(condition.leftright == LEFT) // lodpop on the left
			{
				// lodpop_actor->SetActorLocation(FVector(300.0f, -100.0f, 64.0f));
				debug_msg += "LEFT ";
				out_screenshot_name += "_LEFT_";
			}

			else if(condition.leftright == RIGHT) // lodpop on the right
			{
				// lodpop_actor->SetActorLocation(FVector(300.0f, 100.0f, 64.0));
				debug_msg += "RIGHT ";
				out_screenshot_name += "_RIGHT_";
			}

			else if(condition.leftright == CENTER)
			{
				debug_msg += "CENTER ";
				out_screenshot_name += "_CENTER_";
			}

			if(render_control)
			{
				debug_msg += "CONTROL ";
				out_screenshot_name += "_CONTROL_";
			}

			timerinfo.runtimer                  = true;
			timerinfo.condition_reuse_lightmaps = reuse_lightmaps;

			// Add on the lod model levels
			if(increase_lod)
			{
				out_screenshot_name += "from_" + FString::FromInt(condition.lodlvl) + "_to_" + FString::FromInt(condition.lodlvl + 1);
			}

			else
			{
				out_screenshot_name += "from_" + FString::FromInt(condition.lodlvl) + "_to_" + FString::FromInt(condition.lodlvl - 1);

			}
		}

		else
		{
			UE_LOG(LogTemp, Log, TEXT("control or lodpop meshes are nullptr\n"));

			if(num_conditions_left > 0)
			{
				execute_trial();
			}
		}
	}


	else
	{
		UE_LOG(LogTemp, Log, TEXT("Parameter state not yet initialized, keyboard input will not execute trials\n"));
	}
}



void AExperiment1::increment_lod(UStaticMeshComponent *lod_object, const bool share_lightmap) //, TStructOnScope<FActorComponentInstanceData> &component_instance_data)
{
	UE_LOG(LogTemp, Log, TEXT("increment_lod called\n"));
	// UStaticMeshComponent::ForcedLodModel: If 0, auto-select LOD level. if > 0, force to (ForcedLodModel-1)
	// so add 1 at the end
	const int32_t forced_lod_model      = lod_object->ForcedLodModel - 1;
	int32_t       forced_next_lod_model = ((forced_lod_model + 1) % NUM_LODS) + 1;
	int32_t       next_lod_level        = forced_next_lod_model - 1;

	debug_msg += "from " + FString::FromInt(forced_next_lod_model - 2) + " to " + FString::FromInt(forced_next_lod_model - 1);
	// out_screenshot_name += "_from_" + FString::FromInt(forced_next_lod_model - 2) + "_to_" + FString::FromInt(forced_next_lod_model - 1);

	lod_object->SetForcedLodModel(forced_next_lod_model);

	if(share_lightmap)
	{
		FMeshMapBuildData *meshmap_build_data      = get_mesh_build_data(lod_object, next_lod_level);
		FMeshMapBuildData *meshmap_build_data_at_0 = get_mesh_build_data(lod_object, 0);
		FLightMap2D       *lightmap                = meshmap_build_data_at_0->LightMap->GetLightMap2D();

		if(MIPMAP_SHARED_LIGHTMAP && lightmap)
		{
			if(lightmap->Textures[0])
			{
				lightmap->Textures[0]->LODBias = next_lod_level;
			}

			if(lightmap->Textures[1])
			{
				lightmap->Textures[1]->LODBias = next_lod_level;
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("%s\n"), *debug_msg);
}


void AExperiment1::decrement_lod(UStaticMeshComponent *lod_object, const bool share_lightmap)
{
	UE_LOG(LogTemp, Log, TEXT("decrement_lod called\n"));

	const int32_t forced_lod_model = lod_object->ForcedLodModel - 1;
	int32_t       forced_next_lod_model = ((forced_lod_model - 1) % NUM_LODS) + 1;
	int32_t       next_lod_level        = forced_next_lod_model - 1;

	debug_msg += "from " + FString::FromInt(next_lod_level + 1) + " to " + FString::FromInt(next_lod_level);
	// out_screenshot_name += "_from_" + FString::FromInt(forced_next_lod_model - 2) + "_to_" + FString::FromInt(forced_next_lod_model - 1);

	lod_object->SetForcedLodModel(forced_next_lod_model);

	if(share_lightmap)
	{
		FMeshMapBuildData *meshmap_build_data      = get_mesh_build_data(lod_object, next_lod_level);
		FMeshMapBuildData *meshmap_build_data_at_0 = get_mesh_build_data(lod_object, 0);
		FLightMap2D       *lightmap                = meshmap_build_data_at_0->LightMap->GetLightMap2D();

		if(MIPMAP_SHARED_LIGHTMAP && lightmap)
		{
			if(lightmap->Textures[0])
			{
				lightmap->Textures[0]->LODBias = next_lod_level;
			}

			if(lightmap->Textures[1])
			{
				lightmap->Textures[1]->LODBias = next_lod_level;
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("%s\n"), *debug_msg);
}



void AExperiment1::set_experiment_params()
{
	experiment_params_list.Empty();
	const float eccentricities[NUM_ECCENTRICITIES]     = {3.0f, 11.0f, 22.0f, 30.0f};      // eccentricities in angles
	const float distance_list_increment[NUM_DISTANCES] = {150.0f, 250.0f, 350.0f, 450.0f}; // Starting distance for models moving away. index into this based on lodlvl
	const float model_movement_speeds[NUM_DISTANCES]   = {30.0f, 50.0f, 70.0f, 90.0f};


	ModelID model_id = CASTLE_STRUCTURE;


	if(increase_lod)
	{
		for(uint8_t lodlvl = 0; lodlvl < NUM_LODS - 1; lodlvl++)
		{
			for(uint32_t method = 0; method < METHOD_COUNT; method++)
			{
				ExperimentalParams params = {
					.model_id             = pick_model_params(static_cast<ModelID>(model_id)),
					.lodlvl               = lodlvl,
					.method               = static_cast<Method>(method),
					.leftright            = static_cast<LeftRight>(CENTER),
					.eccentricity         = eccentricities[0],
					.distance             = distance_list_increment[lodlvl],
					.model_movement_speed = model_movement_speeds[lodlvl],
				};

				experiment_params_list.Add(params);
			}
		}
	}

	else
	{
		for(uint8_t lodlvl = NUM_LODS - 1; lodlvl > 0; lodlvl--)
		{
			for(uint32_t method = 0; method < METHOD_COUNT; method++)
			{
				ExperimentalParams params = {
					.model_id             = pick_model_params(static_cast<ModelID>(model_id)),
					.lodlvl               = lodlvl,
					.method               = static_cast<Method>(method),
					.leftright            = static_cast<LeftRight>(CENTER),
					.eccentricity         = eccentricities[0],
					.distance             = distance_list_increment[lodlvl],
					.model_movement_speed = model_movement_speeds[lodlvl],
				};

				experiment_params_list.Add(params);
			}
		}
	}


	for(size_t i = 0; i < experiment_params_list.Num(); i++)
	{
		UE_LOG(LogTemp, Log, TEXT("%d\n"), i);
		UE_LOG(LogTemp, Log, TEXT("\tmodel_id.id: %d\n"), experiment_params_list[i].model_id.id);
		UE_LOG(LogTemp, Log, TEXT("\tlodlvl: %d\n"), experiment_params_list[i].lodlvl);
		UE_LOG(LogTemp, Log, TEXT("\tmethod: %d\n"), experiment_params_list[i].method);
		UE_LOG(LogTemp, Log, TEXT("\tleftright: %d\n"), experiment_params_list[i].leftright);
		UE_LOG(LogTemp, Log, TEXT("\teccentricity: %f\n"), experiment_params_list[i].eccentricity);
	}

	UE_LOG(LogTemp, Log, TEXT("render_control: %d\n"), render_control);
	UE_LOG(LogTemp, Log, TEXT("increase_lod: %d\n"), increase_lod);
}

ModelParams AExperiment1::pick_model_params(ModelID model_id)
{
	// Adjust scales later
	ModelParams params;
	params.id = model_id;
	switch(model_id)
	{
		case CLAYPOT:
			params.scale = 1.0f;
			break;
		case WOOD_LOGS:
			params.scale = 1.0f;
			break;
		case CASTLE_STRUCTURE:
			params.scale = 1.0f;
			break;
		case CLAYJUG:
			params.scale = 1.0f;
			break;
		case ICELANDIC_SPIRE:
			params.scale = 1.0f;
			break;
		case VOLCANIC_ROCK:
			params.scale = 0.3f;
			break;
		case PINE_CONE:
			params.scale = 10.0f;
			break;
		case WOOD_STUMP:
			params.scale = 0.12f;
			break;
		default: // something went wrong
			params.id    = MODELID_COUNT;
			params.scale = 100.0f;
			break;
	}

	return params;
}


FVector AExperiment1::compute_model_position(float distance, float eccentricity, LeftRight side)
{
	// Unreal uses x -> forward, z -> up, y -> left right, lefthanded coordinate system
	// Start straight ahead of the camera, z units away
	FVector position = FVector(distance, 0.0f, 54.0f);
	// All this needs to do for Experiment1 is distance.
	// eccentricity     = side == LEFT ? eccentricity : -eccentricity;

	// Rotate about the z axis
	// position.RotateAngleAxis(eccentricity, FVector(0.0f, 0.0f, 1.0f));


	return position;
}