// Fill out your copyright notice in the Description page of Project Settings.


#include "ExperimentalSetup2.h"
#include "Engine/MapBuildDataRegistry.h"
#include "LODUtils.h"
#include "StaticMeshComponentLODInfo.h"

#include "Lightmap.h"


// Sets default values
AExperimentalSetup2::AExperimentalSetup2()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	set_experiment_params();
}

// Called when the game starts or when spawned
void AExperimentalSetup2::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AExperimentalSetup2::Tick(float DeltaTime)
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
		FVector control_position = control_actor->GetActorLocation();
		float   speed            = 50.0f;
		float   distance_to_move = speed * DeltaTime;
		lodpop_position.X -= distance_to_move;
		control_position.X -= distance_to_move;
		lodpop_actor->SetActorLocation(lodpop_position);
		control_actor->SetActorLocation(control_position);


		if(timerinfo.dt >= lodpop_time && !timerinfo.lodpopped_already)
		{
			increment_lod(lodpop_mesh, timerinfo.condition_reuse_lightmaps);
			increment_lod(control_mesh, timerinfo.condition_reuse_lightmaps);
			timerinfo.lodpopped_already = true;
		}

		if(timerinfo.dt >= timerinfo.disptime)
		{
			timerinfo.reset();
		}
	}


	num_ticks++;
}


// Called to bind functionality to input
void AExperimentalSetup2::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("ExecuteTrial", IE_Released, this, &AExperimentalSetup2::execute_trial);
}


UStaticMeshComponent *AExperimentalSetup2::find_actor_static_mesh_component(TArray<UStaticMeshComponent *> static_mesh_components, AActor *actor)
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


void AExperimentalSetup2::execute_trial() // ExperimentalParams param)
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

		lodpop_actor  = nullptr;
		control_actor = nullptr;
		lodpop_mesh   = nullptr;
		control_mesh  = nullptr;

		for(AActor *actor : GetWorld()->GetLevel(0)->Actors)
		{
			if(actor)
			{
				FString actor_name = actor->GetActorLabel(false);

				if(actor_name.Contains(basemodelname) && actor_name.Contains(methodname))
				{
					if(actor_name.Contains("CONTROL"))
					{
						control_actor = actor;
					}

					else
					{
						lodpop_actor = actor;
					}
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

		if(lodpop_actor != nullptr && control_actor != nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("Lod pop actor: %s\n"), *lodpop_actor->GetActorLabel(false));
			UE_LOG(LogTemp, Log, TEXT("Control actor: %s\n"), *control_actor->GetActorLabel(false));

			TArray<UStaticMeshComponent *> static_mesh_list = condition.method == UNIQUE ? unique_static_meshes : reuse_static_meshes;
			control_mesh                                    = find_actor_static_mesh_component(static_mesh_list, control_actor);
			lodpop_mesh                                     = find_actor_static_mesh_component(static_mesh_list, lodpop_actor);

			// Set appropriate lodlvl
			control_mesh->SetForcedLodModel(condition.lodlvl + 1); // +1 since 0 is auto
			lodpop_mesh->SetForcedLodModel(condition.lodlvl + 1);

			if(condition.leftright == LEFT) // lodpop on the left
			{
				control_actor->SetActorLocation(FVector(300.0f, 100.0f, 64.0f));
				lodpop_actor->SetActorLocation(FVector(300.0f, -100.0f, 64.0f));
			}

			else // lodpop on the right
			{
				control_actor->SetActorLocation(FVector(300.0f, -100.0f, 64.0f));
				lodpop_actor->SetActorLocation(FVector(300.0f, 100.0f, 64.0));
			}

			timerinfo.runtimer                  = true;
			timerinfo.condition_reuse_lightmaps = reuse_lightmaps;

			/*if(lodpop_mesh != nullptr && condition.method == REUSE)
			{

			UE_LOG(LogTemp, Log, TEXT("Before bind function, reuse_lightmaps: %d\n"), reuse_lightmaps);
			lodpop_timer_delegate.BindUFunction(this, "increment_lod", lodpop_mesh, reuse_lightmaps);
			UE_LOG(LogTemp, Log, TEXT("After bind function\n"));

			GetWorld()->GetTimerManager().SetTimer(lodpop_timer, lodpop_timer_delegate, 2.0f, false);
			//increment_lod(lodpop_mesh, reuse_lightmaps);
			}*/
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



void AExperimentalSetup2::increment_lod(UStaticMeshComponent *lod_object, const bool share_lightmap) //, TStructOnScope<FActorComponentInstanceData> &component_instance_data)
{
	UE_LOG(LogTemp, Log, TEXT("increment_lod called\n"));
	// UStaticMeshComponent::ForcedLodModel: If 0, auto-select LOD level. if > 0, force to (ForcedLodModel-1)
	// so add 1 at the end
	const int32_t forced_lod_model      = lod_object->ForcedLodModel - 1;
	int32_t       forced_next_lod_model = ((forced_lod_model + 1) % NUM_LODS) + 1;
	int32_t       next_lod_level        = forced_next_lod_model - 1;

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
}



void AExperimentalSetup2::set_experiment_params()
{
	experiment_params_list.Empty();
	const float eccentricities[NUM_ECCENTRICITIES] = {3.0f, 11.0f, 22.0f, 30.0f};

	for(uint32_t model_id = 0; model_id < MODELID_COUNT; model_id++)
	{
		for(uint8_t lodlvl = 0; lodlvl < NUM_LODS; lodlvl++)
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

ModelParams AExperimentalSetup2::pick_model_params(ModelID model_id)
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