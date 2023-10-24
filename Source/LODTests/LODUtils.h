#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"


void increment_lod(UStaticMesh *lod_object);

void print_lod_info(UStaticMesh *lod_object);

TArray<AActor*> get_static_mesh_actors(UWorld *world);
