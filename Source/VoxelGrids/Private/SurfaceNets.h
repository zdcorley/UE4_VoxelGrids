#pragma once

#include "VoxelGridsPCH.h"
#include "VoxelChunkComponent.h"
#include "VoxelGridComponent.h"
#include <math.h>

class UVoxelChunkComponent;

class VOXELGRIDS_API SurfaceNets : public PolygonizationScheme
{
public:

	/*
	 *	Based on "SurfaceNets in JavaScript" by Mikola Lysenko
	 *
	 *	Used under the MIT License
	 *
	 *	Original code Based on: S.F. Gibson, "Constrained Elastic Surface Nets". (1998) MERL Tech Report.
	 *
	 *	https://github.com/mikolalysenko/mikolalysenko.github.com/blob/master/Isosurface/js/surfacenets.js
	 *
	 *	https://0fps.net/2012/07/12/smooth-voxel-terrain-part-2/
	 *
	 */

	int32 cube_edges[24];
	int32 edge_table[256];

	int32 *idxBuffer;
	int32 buffer_size;

	SurfaceNets()
	{
		// Initialize edge table

		int32 k = 0;
		for (int32 i = 0; i < 8; i++)
		{
			for (int32 j = 1; j <= 4; j <<= 1)
			{
				int32 p = i^j;
				if (i <= p)
				{
					cube_edges[k++] = i;
					cube_edges[k++] = p;
				}
			}
		}

		// Initialize intersection table
		for (int32 i = 0; i < 256; i++)
		{
			int32 em = 0;
			for (int32 j = 0; j < 24; j += 2)
			{
				int32 a = !!(i & (1 << cube_edges[j]));
				int32 b = !!(i & (1 << cube_edges[j + 1]));
				em |= a != b ? (1 << (j >> 1)) : 0;
			}
			edge_table[i] = em;
		}

		idxBuffer = new int32[4096];
		buffer_size = 4096;
	}

	void Polygonize(UVoxelChunkComponent *chunk, bool smooth, MeshData &data) override
	{
		int32 n = 0;
		int32 x[3];
		// this is +1 so we can polygonize the edge cases in which this chunk only owns part of the voxels needed to polygonize.
		int32 sideLength = chunk->GetSideLength() + 1;
		int32 R[3] = { 1, (sideLength + 1), (sideLength + 1) * (sideLength + 1) };
		float grid[8];
		int buf_no = 1;

		// resize if the chunk size is too big
		if (R[2] * 2 > buffer_size)
		{
			delete idxBuffer;
			idxBuffer = new int32[R[2] * 2];
		}

		//March
		for (x[2] = 0; x[2] < sideLength - 1; x[2]++, n += sideLength, buf_no ^= 1, R[2] = -R[2])
		{
			// m is the pointer into the index buffer
			int32 m = 1 + (sideLength + 1) * (1 + buf_no * (sideLength + 1));

			for(x[1]=0; x[1]<sideLength-1; x[1]++, n++, m+=2)
			for (x[0] = 0; x[0] < sideLength - 1; x[0]++, n++, m++)
			{
				// 8 bit mask like in marching cubes
				int32 mask = 0;
				int32 g = 0;
				int32 idx = n;
				for(int32 k = 0; k<2; k++, idx += sideLength*(sideLength-2))
					for(int32 j = 0; j<2; j++, idx += sideLength-2)
						for (int32 i = 0; i < 2; i++, g++, idx++)
						{
							// Lets see if this is an edge case. If so, we're gonna have to sample from another chunk
							

							int32 p = chunk->GetVoxel(idx).health;
							grid[g] = p;
							mask |= (p < 0) ? (1 << g) : 0;
						}

				if (mask == 0 || mask == 0xff)
				{
					continue;
				}

				int32 edge_mask = edge_table[mask];
				float v[3] = { 0.0f, 0.0f, 0.0f };
				int32 e_count = 0;

				for (int32 i = 0; i < 12; i++)
				{
					if (!(edge_mask & (1 << i)))
					{
						continue;
					}

					e_count++;

					int32 e0 = cube_edges[i << 1];
					int32 e1 = cube_edges[(i << 1) + 1];
					float g0 = grid[e0];
					float g1 = grid[e1];
					float t = g0 - g1;

					if (fabs(t) > 0.000001)
					{
						t = g0 / t;
					}
					else
					{
						continue;
					}

					for (int j = 0, k = 1; j < 3; j++, k <<= 1)
					{
						int a = e0 & k;
						int b = e1 & k;
						if (a != b)
						{
							v[j] += a ? 1.0 - t : t;
						}
						else
						{
							v[j] += a ? 1.0 : 0;
						}
					}
				}

				float s = 1.0f / e_count;
				for (int i = 0; i < 3; i++)
				{
					v[i] = x[i] + s * v[i];
				}

				idxBuffer[m] = data.vertices.Num();
				data.vertices.Push(FVector(v[0], v[1], v[2]) * chunk->GetParentGrid()->voxelSize);
				data.vertexColors.Push(FLinearColor(1.0f, 0.0f, 0.0f));

				for (int i = 0; i < 3; i++)
				{
					if (!(edge_mask & (1 << i)))
					{
						continue;
					}

					int iu = (i + 1) % 3;
					int iv = (i + 2) % 3;

					if(x[iu] == 0 || x[iv] == 0)
						continue;

					int du = R[iu];
					int dv = R[iv];

					if (mask & 1)
					{
						data.triangles.Push(idxBuffer[m]);
						data.triangles.Push(idxBuffer[m-du]);
						data.triangles.Push(idxBuffer[m-dv]);

						data.triangles.Push(idxBuffer[m-du]);
						data.triangles.Push(idxBuffer[m-du-dv]);
						data.triangles.Push(idxBuffer[m-dv]);

						//dumb way of calculating UVs
						data.UV0.Push(FVector2D(1.0f, 1.0f));
						data.UV0.Push(FVector2D(0.0f, 1.0f));
						data.UV0.Push(FVector2D(1.0f, 0.0f));

						data.UV0.Push(FVector2D(0.0f, 1.0f));
						data.UV0.Push(FVector2D(0.0f, 0.0f));
						data.UV0.Push(FVector2D(1.0f, 0.0f));

					}
					else
					{
						data.triangles.Push(idxBuffer[m]);
						data.triangles.Push(idxBuffer[m - dv]);
						data.triangles.Push(idxBuffer[m - du]);

						data.triangles.Push(idxBuffer[m - dv]);
						data.triangles.Push(idxBuffer[m - du - dv]);
						data.triangles.Push(idxBuffer[m - du]);

						//dumb way of calculating UVs
						data.UV0.Push(FVector2D(1.0f, 1.0f));
						data.UV0.Push(FVector2D(1.0f, 0.0f));
						data.UV0.Push(FVector2D(0.0f, 1.0f));

						data.UV0.Push(FVector2D(1.0f, 0.0f));
						data.UV0.Push(FVector2D(0.0f, 0.0f));
						data.UV0.Push(FVector2D(0.0f, 1.0f));
					}
				}
			}
		}
	}

};