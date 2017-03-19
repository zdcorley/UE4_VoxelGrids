// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VoxelChunkComponent.h"
#include "Components/SceneComponent.h"
#include "VoxelGridComponent.generated.h"

UENUM(Blueprintable)
enum class EPolygonizationSchemes : uint8 
{
	SurfaceNets,
	MarchingCubes
};

UCLASS(ClassGroup = (Rendering), meta = (BlueprintSpawnableComponent))
class VOXELGRIDS_API UVoxelGridComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVoxelGridComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FVoxel GetVoxel(FVector worldLocation);
	FVoxel GetVoxelLocal(FVector localLocation);

	void SetVoxel(FVector worldLocation, FVoxel voxel);
	void SetVoxelLocal(FVector localLocation, FVoxel voxel);

	// Convert worldspace coordinates to chunkIndex coords
	UVoxelChunkComponent *GetChunkAtWorldCoords(FVector worldLocation);
	UVoxelChunkComponent *GetChunkAtLocalCoords(FVector localLocation);

	void CreateChunkAt(int32 x, int32 y, int32 z);

	UPROPERTY(EditAnywhere)
	EPolygonizationSchemes polygonizationScheme;

	// in voxels
	UPROPERTY(EditAnywhere)
	int32 chunkSize;

	UPROPERTY(EditAnywhere)
	float voxelSize;

	UPROPERTY(EditAnywhere)
	bool smoothLighting;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TMap<FIntVector, UVoxelChunkComponent*> m_chunkMap;
	PolygonizationScheme *m_polyScheme;

};
