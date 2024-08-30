// Fill out your copyright notice in the Description page of Project Settings.


#include "ExperimentalSetup3.h"
#include "Engine/MapBuildDataRegistry.h"
#include "LODUtils.h"
#include "StaticMeshComponentLODInfo.h"

#include "Lightmap.h"


// Sets default values
AExperimentalSetup3::AExperimentalSetup3()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	set_experiment_params();
}

// Called when the game starts or when spawned
void AExperimentalSetup3::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AExperimentalSetup3::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if(num_ticks == 300)
	{
		reuse_static_meshes  = get_static_mesh_components(GetWorld());
		unique_static_meshes = get_static_mesh_components_unique_lightmaps(GetWorld());

		initialized = true;

		UE_LOG(LogTemp, Log, TEXT("Scene setup initalized\n"));
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
		timerinfo.dt += DeltaTime;
		float   lodpop_time      = 1.0f;
		FVector lodpop_position  = lodpop_actor->GetActorLocation();
		float   speed            = 50.0f;
		float   distance_to_move = speed * DeltaTime;
		lodpop_position.X -= distance_to_move;
		lodpop_actor->SetActorLocation(lodpop_position);

		if(timerinfo.dt >= lodpop_time && !timerinfo.lodpopped_already)
		{
			increment_lod(lodpop_mesh, timerinfo.condition_reuse_lightmaps);
			timerinfo.lodpopped_already = true;
		}
		
		out_screenshot_name += "_" + FString::FromInt(timerinfo.screenshot_framenum);
		const FString ImageDirectory = FString::Printf(TEXT("%s/Screenshots/"), *FPaths::ProjectDir());
		FScreenshotRequest::RequestScreenshot(ImageDirectory + out_screenshot_name, false, false);

		timerinfo.screenshot_framenum++;

		if(timerinfo.dt >= timerinfo.disptime)
		{
			timerinfo.reset();
			out_screenshot_name = "";
			debug_msg = "";
		}
	}


	num_ticks++;
}


// Called to bind functionality to input
void AExperimentalSetup3::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("ExecuteTrial", IE_Released, this, &AExperimentalSetup3::execute_trial);
}


UStaticMeshComponent *AExperimentalSetup3::find_actor_static_mesh_component(TArray<UStaticMeshComponent *> static_mesh_components, AActor *actor)
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


void AExperimentalSetup3::execute_trial() // ExperimentalParams param)
{
	if(initialized)
	{
		UE_LOG(LogTemp, Log, TEXT("execute_trial called\n"));
		size_t             num_conditions_left = experiment_params_list.Num();
		size_t             condition_num       = FMath::RandRange(0, (int32_t) num_conditions_left);
		size_t             show_lodpop_first   = FMath::RandRange(0, 1);
		ExperimentalParams condition           = experiment_params_list[condition_num];
		ModelParams        params              = condition.model_id;
		experiment_params_list.RemoveAt(condition_num);

		FString methodname      = condition.method == UNIQUE ? "UNIQUE" : "REUSE";
		bool    reuse_lightmaps = condition.method == UNIQUE ? false : true;
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
				basemodelname = "Castle";
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

				if(actor_name.Contains(basemodelname) && actor_name.Contains(methodname))
				{

					lodpop_actor = actor;
				}

				else
				{
					if(actor->IsRootComponentMovable())
					{
						actor->SetActorLocation(FVector(-500.0f, -500.0f, 64.0f));
					}
				}
			}
		}

		if(lodpop_actor != nullptr)
		{
			debug_msg += basemodelname + " " + methodname + " ";
			out_screenshot_name += basemodelname + "_" + methodname;

			UE_LOG(LogTemp, Log, TEXT("Lod pop actor: %s\n"), *lodpop_actor->GetActorLabel(false));

			TArray<UStaticMeshComponent *> static_mesh_list = condition.method == UNIQUE ? unique_static_meshes : reuse_static_meshes;
			lodpop_mesh                                     = find_actor_static_mesh_component(static_mesh_list, lodpop_actor);

			// Set appropriate lodlvl
			lodpop_mesh->SetForcedLodModel(condition.lodlvl + 1);

			if(condition.leftright == LEFT) // lodpop on the left
			{
				lodpop_actor->SetActorLocation(FVector(300.0f, -100.0f, 64.0f));
				debug_msg += "LEFT ";
				out_screenshot_name += "_LEFT_";
			}

			else // lodpop on the right
			{
				lodpop_actor->SetActorLocation(FVector(300.0f, 100.0f, 64.0));
				debug_msg += "RIGHT ";
				out_screenshot_name += "_RIGHT_";
			}

			timerinfo.runtimer                  = true;
			timerinfo.condition_reuse_lightmaps = reuse_lightmaps;
			UE_LOG(LogTemp, Log, TEXT("Test\n"));


			// Add on the lod model levels
			out_screenshot_name += "from_" + FString::FromInt(condition.lodlvl) + "_to_" + FString::FromInt(condition.lodlvl + 1);
		}

		else
		{
			UE_LOG(LogTemp, Log, TEXT("control or lodpop meshes are nullptr\n"));
		}
	}


	else
	{
		UE_LOG(LogTemp, Log, TEXT("Parameter state not yet initialized, keyboard input will not execute trials\n"));
	}
}



void AExperimentalSetup3::increment_lod(UStaticMeshComponent *lod_object, const bool share_lightmap) //, TStructOnScope<FActorComponentInstanceData> &component_instance_data)
{
	UE_LOG(LogTemp, Log, TEXT("increment_lod called\n"));
	// UStaticMeshComponent::ForcedLodModel: If 0, auto-select LOD level. if > 0, force to (ForcedLodModel-1)
	// so add 1 at the end
	const int32_t forced_lod_model      = lod_object->ForcedLodModel - 1;
	int32_t       forced_next_lod_model = ((forced_lod_model + 1) % NUM_LODS) + 1;
	int32_t       next_lod_level        = forced_next_lod_model - 1;

	debug_msg += "from " + FString::FromInt(forced_next_lod_model - 2) + " to " + FString::FromInt(forced_next_lod_model - 1);
	//out_screenshot_name += "_from_" + FString::FromInt(forced_next_lod_model - 2) + "_to_" + FString::FromInt(forced_next_lod_model - 1);

	lod_object->SetForcedLodModel(forced_next_lod_model);

	if(share_lightmap)
	{
		FMeshMapBuildData *meshmap_build_data = get_mesh_build_data(lod_object, next_lod_level);
		FLightMap2D       *lightmap           = (meshmap_build_data && meshmap_build_data->LightMap) ?
													meshmap_build_data->LightMap->GetLightMap2D() :
													nullptr;

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



void AExperimentalSetup3::set_experiment_params()
{
	experiment_params_list.Empty();
	const float eccentricities[NUM_ECCENTRICITIES] = {3.0f, 11.0f, 22.0f, 30.0f};


	ModelID model_id = CLAYPOT;

	for(uint8_t lodlvl = 0; lodlvl < NUM_LODS - 1; lodlvl++)
	{
		for(uint32_t method = 0; method < METHOD_COUNT; method++)
		{
			for(uint32_t leftright = 0; leftright < LEFTRIGHT_COUNT; leftright++)
			{
				for(uint32_t eccentricity_idx = 0; eccentricity_idx < NUM_ECCENTRICITIES; eccentricity_idx++)
				{
					ExperimentalParams params = {
						.model_id     = pick_model_params(static_cast<ModelID>(model_id)),
						.lodlvl       = lodlvl,
						.method       = static_cast<Method>(method),
						.leftright    = static_cast<LeftRight>(leftright),
						.eccentricity = eccentricities[eccentricity_idx],
					};

					experiment_params_list.Add(params);
				}
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
}

ModelParams AExperimentalSetup3::pick_model_params(ModelID model_id)
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
		default: // something went wrong
			params.id    = MODELID_COUNT;
			params.scale = 100.0f;
			break;
	}

	return params;
}