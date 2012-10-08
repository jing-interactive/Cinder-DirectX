//--------------------------------------------------------------------------------------
// File: CommonStates.h
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
//--------------------------------------------------------------------------------------

#pragma once

#include <d3d11.h>

namespace cinder { namespace dx11 
{
	class CommonStates
	{
	public:
		// Blend states.
		static ID3D11BlendState* Opaque();
		static ID3D11BlendState* AlphaBlend();
		static ID3D11BlendState* Additive();
		static ID3D11BlendState* NonPremultiplied();

		// Depth stencil states.
		static ID3D11DepthStencilState* DepthNone();
		static ID3D11DepthStencilState* DepthDefault();
		static ID3D11DepthStencilState* DepthRead();

		// Rasterizer states.
		static ID3D11RasterizerState* CullNone();
		static ID3D11RasterizerState* CullClockwise();
		static ID3D11RasterizerState* CullCounterClockwise();
		static ID3D11RasterizerState* Wireframe();

		// Sampler states.
		static ID3D11SamplerState* PointWrap();
		static ID3D11SamplerState* PointClamp();
		static ID3D11SamplerState* LinearWrap();
		static ID3D11SamplerState* LinearClamp();
		static ID3D11SamplerState* AnisotropicWrap();
		static ID3D11SamplerState* AnisotropicClamp();
	};
}}
