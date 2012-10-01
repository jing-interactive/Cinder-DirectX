#pragma once

#include "dx11/d3dx11effect.h"

namespace cinder { namespace dx11 {

template <typename VertexType, D3D_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST>
class SimpleVertexBuffer
{
private:
	ID3D11InputLayout*	pInputLayout;
	ID3D11Buffer*		pVertexBuffer;
public:
	SimpleVertexBuffer():pInputLayout(NULL),pVertexBuffer(NULL){}

	void reset()
	{
		SAFE_RELEASE(pInputLayout);
		SAFE_RELEASE(pVertexBuffer);
	}

	~SimpleVertexBuffer()
	{
		reset();
	}

	HRESULT createInputLayout(dx11::HlslEffect& effect, int pass = 0)
	{
		SAFE_RELEASE(pInputLayout);
		return effect.createInputLayout(VertexType::InputElements, VertexType::InputElementCount, &pInputLayout, pass) ;
	}

	HRESULT createBuffer(const VertexType* pVertices, int nVertices)
	{
		SAFE_RELEASE(pVertexBuffer);
		CD3D11_BUFFER_DESC bd(sizeof(VertexType)*nVertices, D3D11_BIND_VERTEX_BUFFER);

		D3D11_SUBRESOURCE_DATA InitData = {0};
		InitData.pSysMem = pVertices;
		return dx11::getDevice()->CreateBuffer( &bd, &InitData, &pVertexBuffer );
	}

	void bind()
	{
		dx11::getImmediateContext()->IASetInputLayout(pInputLayout);
		dx11::getImmediateContext()->IASetPrimitiveTopology( Topology );

		UINT stride = sizeof( VertexType);
		UINT offset = 0;
		dx11::getImmediateContext()->IASetVertexBuffers( 0, 1, &pVertexBuffer, &stride, &offset );
	}
};

}}