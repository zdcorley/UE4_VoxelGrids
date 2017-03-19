// Fill out your copyright notice in the Description page of Project Settings.

#include "VoxelGridsPCH.h"

#include "VoxelGrids.h"
#include "VoxelGridComponent.h"

// Sets default values for this component's properties
UVoxelGridComponent::UVoxelGridComponent(const FObjectInitializer& ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	switch (polygonizationScheme)
	{
	case EPolygonizationSchemes::SurfaceNets:
		m_polyScheme = new SurfaceNets();
		break;
	case EPolygonizationSchemes::MarchingCubes:

		break;
	}

}


// Called when the game starts
void UVoxelGridComponent::BeginPlay()
{
	Super::BeginPlay();

	CreateChunkAt(0, 0, 0);
	CreateChunkAt(1, 0, 0);
	CreateChunkAt(0, 1, 0);
	CreateChunkAt(1, 1, 0);

}


// Called every frame
void UVoxelGridComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FVoxel UVoxelGridComponent::GetVoxel(FVector worldLocation)
{
	FVoxel v;
	v.health = 0.0f;
	return v;
}

FVoxel UVoxelGridComponent::GetVoxelLocal(FVector localLocation)
{
	FVoxel v;
	v.health = 0.0f;
	return v;
}

void UVoxelGridComponent::SetVoxel(FVector worldLocation, FVoxel voxel)
{

}

void UVoxelGridComponent::SetVoxelLocal(FVector localLocation, FVoxel voxel)
{

}

UVoxelChunkComponent * UVoxelGridComponent::GetChunkAtWorldCoords(FVector worldLocation)
{
	return nullptr;
}

UVoxelChunkComponent * UVoxelGridComponent::GetChunkAtLocalCoords(FVector localLocation)
{
	return nullptr;
}

void UVoxelGridComponent::CreateChunkAt(int32 x, int32 y, int32 z)
{
	UVoxelChunkComponent *chunk = NewObject<UVoxelChunkComponent>(this);
	chunk->RegisterComponent();
	chunk->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
	chunk->SetRelativeLocation(FVector(x * (chunkSize * voxelSize), y * (chunkSize * voxelSize), z * (chunkSize * voxelSize)));
	chunk->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	chunk->Initialize(chunkSize, FIntVector(x, y, z), this, m_polyScheme);
	chunk->Polygonize(smoothLighting);
	m_chunkMap.Add(FIntVector(x, y, z), chunk);
}
