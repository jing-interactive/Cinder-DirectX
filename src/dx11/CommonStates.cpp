//--------------------------------------------------------------------------------------
// File: CommonStates.cpp
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

#include "dx11/CommonStates.h"
#include "dx11/dx11.h"

namespace cinder { namespace dx11
{
	static ID3D11BlendState* opaque = NULL;
	static ID3D11BlendState* alphaBlend = NULL;
	static ID3D11BlendState* additive = NULL;
	static ID3D11BlendState* nonPremultiplied = NULL;

	static ID3D11DepthStencilState* depthNone = NULL;
	static ID3D11DepthStencilState* depthDefault = NULL;
	static ID3D11DepthStencilState* depthRead = NULL;

	static ID3D11RasterizerState* cullNone = NULL;
	static ID3D11RasterizerState* cullClockwise = NULL;
	static ID3D11RasterizerState* cullCounterClockwise = NULL;
	static ID3D11RasterizerState* wireframe = NULL;

	static ID3D11SamplerState* pointWrap = NULL;
	static ID3D11SamplerState* pointClamp = NULL;
	static ID3D11SamplerState* linearWrap = NULL;
	static ID3D11SamplerState* linearClamp = NULL;
	static ID3D11SamplerState* anisotropicWrap = NULL;
	static ID3D11SamplerState* anisotropicClamp = NULL;

	static struct AutoDeleter
	{
		void release()
		{
			SAFE_RELEASE(opaque);
			SAFE_RELEASE(alphaBlend);
			SAFE_RELEASE(additive);
			SAFE_RELEASE(nonPremultiplied);

			SAFE_RELEASE(depthNone);
			SAFE_RELEASE(depthDefault);
			SAFE_RELEASE(depthRead);

			SAFE_RELEASE(cullNone);
			SAFE_RELEASE(cullClockwise);
			SAFE_RELEASE(cullCounterClockwise);
			SAFE_RELEASE(wireframe);

			SAFE_RELEASE(pointWrap);
			SAFE_RELEASE(pointClamp);
			SAFE_RELEASE(linearWrap);
			SAFE_RELEASE(linearClamp);
			SAFE_RELEASE(anisotropicWrap);
			SAFE_RELEASE(anisotropicClamp);
		}
		~AutoDeleter()
		{
			release();
		}
	}_;

	static HRESULT CreateBlendState(D3D11_BLEND srcBlend, D3D11_BLEND destBlend, _Out_ ID3D11BlendState** pResult);
	static HRESULT CreateDepthStencilState(bool enable, bool writeEnable, _Out_ ID3D11DepthStencilState** pResult);
	static HRESULT CreateRasterizerState(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, _Out_ ID3D11RasterizerState** pResult);
	static HRESULT CreateSamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode, _Out_ ID3D11SamplerState** pResult);

	// Helper for creating blend state objects.
	HRESULT CreateBlendState(D3D11_BLEND srcBlend, D3D11_BLEND destBlend, _Out_ ID3D11BlendState** pResult)
	{
		D3D11_BLEND_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.RenderTarget[0].BlendEnable = (srcBlend != D3D11_BLEND_ONE) ||
			(destBlend != D3D11_BLEND_ZERO);

		desc.RenderTarget[0].SrcBlend  = desc.RenderTarget[0].SrcBlendAlpha  = srcBlend;
		desc.RenderTarget[0].DestBlend = desc.RenderTarget[0].DestBlendAlpha = destBlend;
		desc.RenderTarget[0].BlendOp   = desc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;

		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		return dx11::getDevice()->CreateBlendState(&desc, pResult);
	}


	// Helper for creating depth stencil state objects.
	HRESULT CreateDepthStencilState(bool enable, bool writeEnable, _Out_ ID3D11DepthStencilState** pResult)
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.DepthEnable = enable;
		desc.DepthWriteMask = writeEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		desc.StencilEnable = false;
		desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

		desc.BackFace = desc.FrontFace;

		return dx11::getDevice()->CreateDepthStencilState(&desc, pResult);
	}


	// Helper for creating rasterizer state objects.
	HRESULT CreateRasterizerState(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, _Out_ ID3D11RasterizerState** pResult)
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.CullMode = cullMode;
		desc.FillMode = fillMode;
		desc.DepthClipEnable = true;
		desc.MultisampleEnable = true;

		return dx11::getDevice()->CreateRasterizerState(&desc, pResult);
	}


	// Helper for creating sampler state objects.
	HRESULT CreateSamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode, _Out_ ID3D11SamplerState** pResult)
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.Filter = filter;

		desc.AddressU = addressMode;
		desc.AddressV = addressMode;
		desc.AddressW = addressMode;

		desc.MaxLOD = FLT_MAX;
		desc.MaxAnisotropy = 16;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		return dx11::getDevice()->CreateSamplerState(&desc, pResult);
	}


	ID3D11BlendState* CommonStates::Opaque()
	{
		if (opaque == NULL)
			CreateBlendState(D3D11_BLEND_ONE, D3D11_BLEND_ZERO, &opaque);
		return opaque;
	}


	ID3D11BlendState* CommonStates::AlphaBlend()
	{
		if (alphaBlend == NULL)
			CreateBlendState(D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA, &alphaBlend);
		return alphaBlend;
	}


	ID3D11BlendState* CommonStates::Additive()
	{
		if (additive == NULL)
			CreateBlendState(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_ONE, &alphaBlend);
		return additive;
	}


	ID3D11BlendState* CommonStates::NonPremultiplied()
	{
		if (nonPremultiplied == NULL)
			 CreateBlendState(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, &nonPremultiplied);
		return nonPremultiplied;
	}


	ID3D11DepthStencilState* CommonStates::DepthNone()
	{
		if (depthNone == NULL)
			CreateDepthStencilState(false, false, &depthNone);
		return depthNone;
 	}


	ID3D11DepthStencilState* CommonStates::DepthDefault()
	{
		if (depthDefault == NULL)
			CreateDepthStencilState(true, true, &depthDefault);
		return depthDefault;
	}


	ID3D11DepthStencilState* CommonStates::DepthRead()
	{
		if (depthRead == NULL)
			CreateDepthStencilState(true, false, &depthRead);
		return depthRead;
	}


	ID3D11RasterizerState* CommonStates::CullNone()
	{
		if (cullNone == NULL)
			CreateRasterizerState(D3D11_CULL_NONE, D3D11_FILL_SOLID, &cullNone);
		return cullNone;
	}


	ID3D11RasterizerState* CommonStates::CullClockwise()
	{
		if (cullClockwise == NULL)
			CreateRasterizerState(D3D11_CULL_FRONT, D3D11_FILL_SOLID, &cullClockwise);
		return cullClockwise;
	}


	ID3D11RasterizerState* CommonStates::CullCounterClockwise()
	{
		if (cullCounterClockwise == NULL)
			CreateRasterizerState(D3D11_CULL_BACK, D3D11_FILL_SOLID, &cullCounterClockwise);
		return cullCounterClockwise;
	}


	ID3D11RasterizerState* CommonStates::Wireframe()
	{
		if (wireframe == NULL)
			CreateRasterizerState(D3D11_CULL_BACK, D3D11_FILL_WIREFRAME, &wireframe);
		return wireframe;
	}


	ID3D11SamplerState* CommonStates::PointWrap()
	{
		if (pointWrap == NULL)
			CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP, &pointWrap);
		return pointWrap;
	}


	ID3D11SamplerState* CommonStates::PointClamp()
	{
		if (pointClamp == NULL)
			CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, &pointClamp);
		return pointClamp;
	}


	ID3D11SamplerState* CommonStates::LinearWrap()
	{
		if (linearWrap == NULL)
			CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, &linearWrap);
		return linearWrap;
	}


	ID3D11SamplerState* CommonStates::LinearClamp()
	{
		if (linearClamp == NULL)
			CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, &linearClamp);
		return linearClamp;
	}


	ID3D11SamplerState* CommonStates::AnisotropicWrap()
	{
		if (anisotropicWrap == NULL)
			CreateSamplerState(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, &anisotropicWrap);
		return anisotropicWrap;
	}


	ID3D11SamplerState* CommonStates::AnisotropicClamp()
	{
		if (anisotropicClamp == NULL)
			CreateSamplerState(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_CLAMP, &anisotropicClamp);
		return anisotropicClamp;
	}

}}