//***************************************************************************************
// LightHelper.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper classes for lighting.
//***************************************************************************************

#ifndef LIGHTHELPER_H
#define LIGHTHELPER_H

#include <Windows.h>
#include "cinder/Vector.h"

namespace cinder { namespace dx11{

// Note: Make sure structure alignment agrees with HLSL structure padding rules. 
//   Elements are packed into 4D vectors with the restriction that an element
//   cannot straddle a 4D vector boundary.

struct DirectionalLight
{
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }

	Vec4f Ambient;
	Vec4f Diffuse;
	Vec4f Specular;
	Vec3f Direction;
	float Pad; // Pad the last float so we can set an array of lights if we wanted.
};

struct PointLight
{
	PointLight() { ZeroMemory(this, sizeof(this)); }

	Vec4f Ambient;
	Vec4f Diffuse;
	Vec4f Specular;

	// Packed into 4D vector: (Position, Range)
	Vec3f Position;
	float Range;

	// Packed into 4D vector: (A0, A1, A2, Pad)
	Vec3f Att;
	float Pad; // Pad the last float so we can set an array of lights if we wanted.
};

struct SpotLight
{
	SpotLight() { ZeroMemory(this, sizeof(this)); }

	Vec4f Ambient;
	Vec4f Diffuse;
	Vec4f Specular;

	// Packed into 4D vector: (Position, Range)
	Vec3f Position;
	float Range;

	// Packed into 4D vector: (Direction, Spot)
	Vec3f Direction;
	float Spot;

	// Packed into 4D vector: (Att, Pad)
	Vec3f Att;
	float Pad; // Pad the last float so we can set an array of lights if we wanted.
};

struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); }

	Vec4f Ambient;
	Vec4f Diffuse;
	Vec4f Specular; // w = SpecPower
	Vec4f Reflect;
};

}}
#endif // LIGHTHELPER_H
