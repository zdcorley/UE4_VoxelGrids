// Fill out your copyright notice in the Description page of Project Settings.

#include "VoxelGridsPCH.h"

#include "VoxelGrids.h"
#include "VoxelGridComponent.h"
#include "VoxelChunkComponent.h"
#include "KismetProceduralMeshLibrary.h"

void UVoxelChunkComponent::Initialize(int sideLength, FIntVector mapIndex, UVoxelGridComponent* parentGrid, PolygonizationScheme *polyScheme)
{
	m_voxelArray = new FVoxel[sideLength * sideLength * sideLength];
	for (int i = 0; i < sideLength * sideLength * sideLength; i++)
	{
		FVoxel v;
		v.health = FMath::RandRange(-1, 1);
		m_voxelArray[i] = v;
	}
	m_sideLength = sideLength;
	m_index = mapIndex;
	m_parentGrid = parentGrid;
	m_polyScheme = polyScheme;
	
	FVoxel v;
	v.health = 1.0f;
	//SetVoxel(4, 4, 4, v);
}

void UVoxelChunkComponent::BeginPlay()
{

}

void UVoxelChunkComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	// Do things
}

void UVoxelChunkComponent::Polygonize(bool smooth)
{
	MeshData data;
	m_polyScheme->Polygonize(this, smooth, data);

	if(data.normals.Num() == 0 || data.tangents.Num() == 0)
		UKismetProceduralMeshLibrary::CalculateTangentsForMesh(data.vertices, data.triangles, data.UV0, data.normals, data.tangents);

	CreateMeshSection_LinearColor(1, data.vertices, data.triangles, data.normals, data.UV0, data.vertexColors, data.tangents, false);
}

UVoxelGridComponent *UVoxelChunkComponent::GetParentGrid()
{
	return m_parentGrid;
}

FIntVector UVoxelChunkComponent::GetChunkIndex()
{
	return m_index;
}

FVoxel UVoxelChunkComponent::GetVoxel(int32 x, int32 y, int32 z)
{
	return m_voxelArray[GetVoxelArrayIdx(x, y, z)];
}

FVoxel UVoxelChunkComponent::GetVoxel(int32 idx)
{
	return m_voxelArray[idx];
}

FVoxel UVoxelChunkComponent::GetVoxel(FIntVector idx)
{
	return m_voxelArray[GetVoxelArrayIdx(idx.X, idx.Y, idx.Z)];
}

int32 UVoxelChunkComponent::GetVoxelArrayIdx(int32 x, int32 y, int32 z)
{
	return x + m_sideLength * y + m_sideLength * m_sideLength * z;
}

void UVoxelChunkComponent::SetVoxel(int32 x, int32 y, int32 z, FVoxel v)
{
	m_voxelArray[GetVoxelArrayIdx(x, y, z)] = v;
}

void UVoxelChunkComponent::SetVoxel(FIntVector idx, FVoxel v)
{
	m_voxelArray[GetVoxelArrayIdx(idx.X, idx.Y, idx.Z)] = v;
}

int32 UVoxelChunkComponent::GetSideLength()
{
	return m_sideLength;
}

