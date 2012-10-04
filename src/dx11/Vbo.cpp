#include "dx11/Vbo.h"
#include "dx11/VertexTypes.h"

namespace cinder { namespace dx11 {

VboMesh::VboMesh( const TriMesh &triMesh ):
mObj( std::shared_ptr<Obj>( new Obj ) )
{
	bool N = triMesh.hasNormals();
	bool C = triMesh.hasColorsRGB();
	bool Ca = triMesh.hasColorsRGBA();
	bool T = triMesh.hasTexCoords();

	if (!N && (C || Ca) && !T)
	{//PC
		size_t nVertices = triMesh.getNumVertices();
		std::vector<VertexPC> vertices(nVertices);
		for (size_t i=0;i<nVertices;i++)
		{
			vertices[i].position = triMesh.getVertices()[i];
			if (C)
				vertices[i].color = triMesh.getColorsRGB()[i];
			else
				vertices[i].color = triMesh.getColorsRGBA()[i];
		}
		createBuffer<VertexPC>(&vertices[0], nVertices);
	}

	if (N && (C || Ca) && !T)
	{//PNC
		size_t nVertices = triMesh.getNumVertices();
		std::vector<VertexPNC> vertices(nVertices);
		for (size_t i=0;i<nVertices;i++)
		{
			vertices[i].position = triMesh.getVertices()[i];
			vertices[i].normal = triMesh.getNormals()[i];
			if (C)
				vertices[i].color = triMesh.getColorsRGB()[i];
			else
				vertices[i].color = triMesh.getColorsRGBA()[i];
		}
		createBuffer<VertexPNC>(&vertices[0], nVertices);
	}

	if (N && !(C || Ca) && T)
	{//PNT
		size_t nVertices = triMesh.getNumVertices();
		std::vector<VertexPNT> vertices(nVertices);
		for (size_t i=0;i<nVertices;i++)
		{
			vertices[i].position = triMesh.getVertices()[i];
			vertices[i].normal = triMesh.getNormals()[i];
			vertices[i].texCoord = triMesh.getTexCoords()[i];
		}
		createBuffer<VertexPNT>(&vertices[0], nVertices);
	}

	if (N && T)
	{//PNT
		size_t nVertices = triMesh.getNumVertices();
		std::vector<VertexPNT> vertices(nVertices);
		for (size_t i=0;i<nVertices;i++)
		{
			vertices[i].position = triMesh.getVertices()[i];
			vertices[i].normal = triMesh.getNormals()[i];
			vertices[i].texCoord = triMesh.getTexCoords()[i];
		}
		createBuffer<VertexPNT>(&vertices[0], nVertices);
	}

	//index buffer
	if (triMesh.getNumIndices() > 0)
	{
		createBuffer(&triMesh.getIndices()[0], triMesh.getNumIndices());
	}
}

void VboMesh::bind( D3D_PRIMITIVE_TOPOLOGY Topology /*= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST*/ )
{
	dx11::getImmediateContext()->IASetInputLayout(mObj->pInputLayout);
	dx11::getImmediateContext()->IASetPrimitiveTopology( Topology );

	UINT stride = mObj->vertexSize;
	UINT offset = 0;
	dx11::getImmediateContext()->IASetVertexBuffers( 0, 1, &mObj->pVertexBuffer, &stride, &offset );
	dx11::getImmediateContext()->IASetIndexBuffer(mObj->pIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );
}

HRESULT VboMesh::createBuffer( const UINT* pIndices, UINT nIndices )
{
	CD3D11_BUFFER_DESC bd(sizeof(UINT)*nIndices, D3D11_BIND_INDEX_BUFFER);

	D3D11_SUBRESOURCE_DATA InitData = {0};
	InitData.pSysMem = pIndices;
	return dx11::getDevice()->CreateBuffer( &bd, &InitData, &mObj->pIndexBuffer );
}

HRESULT VboMesh::createInputLayout( dx11::HlslEffect& effect, int pass /*= 0*/ )
{
	assert (mObj->pInputElementDescs != NULL && "call createBuffer() first");
	return effect.createInputLayout(mObj->pInputElementDescs, mObj->NumInputElements, &mObj->pInputLayout, pass) ;
}


VboMesh::Obj::~Obj()
{
	SAFE_RELEASE(pInputLayout);
	SAFE_RELEASE(pVertexBuffer);
	SAFE_RELEASE(pIndexBuffer);
}

}}