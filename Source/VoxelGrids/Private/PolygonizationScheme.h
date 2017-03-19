// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Core.h"
#include "ProceduralMeshComponent.h"

struct MeshData
{
	// MUST be filled by Polygonize implementation
	TArray<FVector> vertices;
	TArray<int32> triangles;
	TArray<FVector2D> UV0;
	TArray<FLinearColor> vertexColors;

	// Optional. If not filled, the chunk will automatically calculate normals after Polygonize is run.
	// NOTE: In most cases it will be faster to generate these in the Polygonize function.
	TArray<FVector> normals;
	TArray<FProcMeshTangent> tangents;
};

class UVoxelChunkComponent;

/**
 * Defines an abstract PolygonizationScheme, which implements an algorithm that takes in raw Voxel or point cloud data,
 * And polygonizes the field, resulting in a filled MeshData structure.
 *
 * Note: For a chunk based grid, the Polygonize implementation must account for chunk edge cases on its own.
 */
class VOXELGRIDS_API PolygonizationScheme
{
public:
	virtual void Polygonize(UVoxelChunkComponent *chunk, bool smooth, MeshData &data) = 0;

	FIntVector Get3DIndex(int32 idx, int32 sideLength)
	{
		int32 zFactor = (sideLength * sideLength);
		int32 z = idx / zFactor;
		int32 y = (idx - z*zFactor) / sideLength;
		int32 x = (idx - z*zFactor - y*sideLength);
		return FIntVector(x, y, z);
	}
private:


};
