// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProceduralMeshComponent.h"
#include "VoxelChunkComponent.generated.h"

class UVoxelGridComponent;

USTRUCT(Blueprintable)
struct FVoxel
{
	GENERATED_USTRUCT_BODY();
	float health;
};
/**
 * 
 */
UCLASS(ClassGroup = (Rendering), meta = (BlueprintSpawnableComponent))
class VOXELGRIDS_API UVoxelChunkComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:

	void Initialize(int sideLength, FIntVector mapIndex, UVoxelGridComponent* parentGrid, PolygonizationScheme *polyScheme);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	void Polygonize(bool smooth);

	UVoxelGridComponent *GetParentGrid();
	FIntVector GetChunkIndex();

	FVoxel GetVoxel(FIntVector idx);
	FVoxel GetVoxel(int32 x, int32 y, int32 z);
	FVoxel GetVoxel(int32 idx);

	int32 GetVoxelArrayIdx(int32 x, int32 y, int32 z);

	void SetVoxel(int32 x, int32 y, int32 z, FVoxel v);
	void SetVoxel(FIntVector idx, FVoxel v);

	int32 GetSideLength();

protected:
	// size
	int m_sideLength;

	// chunk index position in array
	FIntVector m_index;

	UVoxelGridComponent *m_parentGrid;
	PolygonizationScheme *m_polyScheme;
	// single dimension array, converts to 3d array
	FVoxel *m_voxelArray;
	
};
