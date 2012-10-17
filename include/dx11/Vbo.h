#pragma once

#include "dx11/HlslEffect.h"
#include "dx11/dx11.h"

namespace cinder {
	class TriMesh;
}

namespace cinder { namespace dx11 {

class SdkMesh;

template <typename T> 
struct IndexBufferTraits
{
};

template<> 
struct IndexBufferTraits<uint16_t>
{
	static DXGI_FORMAT getDXGIFormat(){return DXGI_FORMAT_R16_UINT;}
};

template<> 
struct IndexBufferTraits<uint32_t>
{
	static DXGI_FORMAT getDXGIFormat(){return DXGI_FORMAT_R32_UINT;}
};

class VboMesh
{
private:
	struct Obj {
		Obj():pInputLayout(NULL), pVertexBuffer(NULL), pIndexBuffer(NULL), mVertexSize(0), mNumIndices(0), mNumVertices(0), pInputElementDescs(NULL), mNumInputElements(0),mIBFormat(DXGI_FORMAT_UNKNOWN){}
		~Obj();

		ID3D11InputLayout*	pInputLayout;
		ID3D11Buffer*		pVertexBuffer;
		ID3D11Buffer*		pIndexBuffer;
		size_t			mVertexSize;
		size_t			mNumIndices;
		size_t			mNumVertices;
		D3D11_INPUT_ELEMENT_DESC *pInputElementDescs; 
		size_t			mNumInputElements;
		DXGI_FORMAT		mIBFormat;
	};

	std::shared_ptr<Obj>	mObj;

public:
	VboMesh(){}

	VboMesh( const TriMesh &triMesh, bool normalMap = false, bool flipOrder = true );
	VboMesh( const SdkMesh &sdkMesh, bool normalMap = false, bool flipOrder = true );

	HRESULT createInputLayout(dx11::HlslEffect& effect, int pass = 0);

	size_t	getNumIndices() const { return mObj->mNumIndices; }
	size_t	getNumVertices() const { return mObj->mNumVertices; }

	template <typename VertexType>
	HRESULT createVertexBuffer(const VertexType* pVertices, UINT nVertices)
	{
		return createVertexBuffer(pVertices, nVertices,
			VertexType::InputElements,  VertexType::InputElementCount,
			sizeof(VertexType));
	}

	HRESULT createVertexBuffer(const void* pVertices, UINT nVertices, const D3D11_INPUT_ELEMENT_DESC* pElementDescs, UINT NumInputElements, UINT VertexSize);

	template <typename IndexType>
	HRESULT createIndexBuffer(const IndexType* pIndices, UINT nIndices)
	{
		CD3D11_BUFFER_DESC bd(sizeof(IndexType)*nIndices, D3D11_BIND_INDEX_BUFFER);

		D3D11_SUBRESOURCE_DATA InitData = {0};
		InitData.pSysMem = pIndices;
		mObj->mNumIndices = nIndices;
		mObj->mIBFormat = IndexBufferTraits<IndexType>::getDXGIFormat();
		return dx11::getDevice()->CreateBuffer( &bd, &InitData, &mObj->pIndexBuffer );
	}

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