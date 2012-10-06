#pragma once

#include "cinder/TriMesh.h"
#include "dx11/HlslEffect.h"
#include "dx11/dx11.h"

namespace cinder { namespace dx11 {

class VboMesh
{
private:
	struct Obj {
		Obj():pInputLayout(NULL), pVertexBuffer(NULL), pIndexBuffer(NULL), vertexSize(0), mNumIndices(0), mNumVertices(0), pInputElementDescs(NULL), NumInputElements(0){}
		~Obj();

		ID3D11InputLayout*	pInputLayout;
		ID3D11Buffer*		pVertexBuffer;
		ID3D11Buffer*		pIndexBuffer;
		UINT			vertexSize;
		size_t			mNumIndices;
		size_t			mNumVertices;
		D3D11_INPUT_ELEMENT_DESC *pInputElementDescs; 
		UINT			NumInputElements;
	};

	std::shared_ptr<Obj>	mObj;

public:
	VboMesh(){}

	VboMesh( const TriMesh &triMesh);

	HRESULT createInputLayout(dx11::HlslEffect& effect, int pass = 0);

	size_t	getNumIndices() const { return mObj->mNumIndices; }
	size_t	getNumVertices() const { return mObj->mNumVertices; }

	template <typename VertexType>
	HRESULT createBuffer(const VertexType* pVertices, UINT nVertices)
	{
		mObj = std::shared_ptr<Obj>( new Obj );

		mObj->pInputElementDescs = (D3D11_INPUT_ELEMENT_DESC*)VertexType::InputElements;
		mObj->NumInputElements = VertexType::InputElementCount;

		mObj->vertexSize = sizeof(VertexType);
		CD3D11_BUFFER_DESC bd(mObj->vertexSize*nVertices, D3D11_BIND_VERTEX_BUFFER);

		D3D11_SUBRESOURCE_DATA InitData = {0};
		InitData.pSysMem = pVertices;
		return dx11::getDevice()->CreateBuffer( &bd, &InitData, &mObj->pVertexBuffer );
	}

	HRESULT createBuffer(const UINT* pIndices, UINT nIndices);

	void bind(D3D_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) const;

public:
	//@{
	//! Emulates shared_ptr-like behavior
	typedef std::shared_ptr<Obj> VboMesh::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &VboMesh::mObj; }
	void reset() { mObj.reset(); }
	//@}  
};

}}