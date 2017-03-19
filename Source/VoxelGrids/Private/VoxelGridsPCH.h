// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "VoxelGrids.h"

#include "PolygonizationScheme.h"
#include "SurfaceNets.h"
// You should place include statements to your module's private header files here.  You only need to
// add includes for headers that are used in most of your module's source files though.

enum VoxelOperationResult
{
	VOR_SUCCESS = 0,
	VOR_UNKNOWN_FAIL = 1
};