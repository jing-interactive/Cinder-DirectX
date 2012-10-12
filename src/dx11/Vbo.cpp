#include "dx11/Vbo.h"
#include "dx11/VertexTypes.h"

namespace cinder { namespace dx11 {

static std::vector<Vec3f> computeTangent(const TriMesh &triMesh)
{
	size_t NumVertices = triMesh.getNumVertices();
	const std::vector<Vec3f>& positions = triMesh.getVertices();
	std::vector<uint32_t> indices = triMesh.getIndices();
	if (indices.empty())
	{//make fake indices
		indices.resize(NumVertices);
		for (size_t i=0;i<NumVertices;i++)
			indices[i] = i;
	}
	size_t NumTriangles = indices.size()/3;

	std::vector<Vec3f> tangents(NumVertices);
	for (size_t i=0;i<NumTriangles;i++)
	{
		uint32_t i0 = indices[i*3+0];
		uint32_t i1 = indices[i*3+1];
		uint32_t i2 = indices[i*3+2];

		Vec3f diff = triMesh.getVertices()[i1] - triMesh.getVertices()[i0]; 
		float u1 = triMesh.getTexCoords()[i1].x - triMesh.getTexCoords()[i0].x;
		Vec3f tangent = diff/u1;
		tangent.normalize();

		tangents[i0] = tangent;
		tangents[i1] = tangent;
		tangents[i2] = tangent;
	}
	return tangents;
}

VboMesh::VboMesh( const TriMesh &triMesh, bool normalMap, bool flipOrder ):
mObj( std::shared_ptr<Obj>( new Obj ) )
{
	bool N = triMesh.hasNormals();
	bool C = triMesh.hasColorsRGB();
	bool Ca = triMesh.hasColorsRGBA();
	bool T = triMesh.hasTexCoords();

	mObj->mNumVertices = triMesh.getNumVertices();

	if (normalMap)
	{//
		assert (N && T);
		std::vector<Vec3f> tangents = computeTangent(triMesh);
		std::vector<VertexNMap> vertices(mObj->mNumVertices);
		for (size_t i=0;i<mObj->mNumVertices;i++)
		{
			vertices[i].position = triMesh.getVertices()[i];
			vertices[i].normal = triMesh.getNormals()[i];
			vertices[i].texCoord = triMesh.getTexCoords()[i];
			vertices[i].tangent = tangents[i];
		}
		createBuffer<VertexNMap>(&vertices[0], mObj->mNumVertices);
	}
	else
	{
		if (!N && (C || Ca) && !T)
		{//PC
			std::vector<VertexPC> vertices(mObj->mNumVertices);
			for (size_t i=0;i<mObj->mNumVertices;i++)
			{
				vertices[i].position = triMesh.getVertices()[i];
				if (C)
					vertices[i].color = triMesh.getColorsRGB()[i];
				else
					vertices[i].color = triMesh.getColorsRGBA()[i];
			}
			createBuffer<VertexPC>(&vertices[0], mObj->mNumVertices);
		}

		if (N && (C || Ca) && !T)
		{//PNC
			std::vector<VertexPNC> vertices(mObj->mNumVertices);
			for (size_t i=0;i<mObj->mNumVertices;i++)
			{
				vertices[i].position = triMesh.getVertices()[i];
				vertices[i].normal = triMesh.getNormals()[i];
				if (C)
					vertices[i].color = triMesh.getColorsRGB()[i];
				else
					vertices[i].color = triMesh.getColorsRGBA()[i];
			}
			createBuffer<VertexPNC>(&vertices[0], mObj->mNumVertices);
		}

		if (N && !(C || Ca) && T)
		{//PNT
			std::vector<VertexPNT> vertices(mObj->mNumVertices);
			for (size_t i=0;i<mObj->mNumVertices;i++)
			{
				vertices[i].position = triMesh.getVertices()[i];
				vertices[i].normal = triMesh.getNormals()[i];
				vertices[i].texCoord = triMesh.getTexCoords()[i];
			}
			createBuffer<VertexPNT>(&vertices[0], mObj->mNumVertices);
		}

		if (!N && !(C || Ca) && T)
		{//PNT
			mObj->mNumVertices = triMesh.getNumVertices();
			std::vector<VertexPT> vertices(mObj->mNumVertices);
			for (size_t i=0;i<mObj->mNumVertices;i++)
			{
				vertices[i].position = triMesh.getVertices()[i];
				vertices[i].texCoord = triMesh.getTexCoords()[i];
			}
			createBuffer<VertexPT>(&vertices[0], mObj->mNumVertices);
		}
	}

	//index buffer
	mObj->mNumIndices = triMesh.getNumIndices();
	if (mObj->mNumIndices > 0)
	{
		if (flipOrder)
		{
			std::vector<uint32_t> transformed_indices(mObj->mNumIndices);
			for (size_t i=0;i<mObj->mNumIndices;i+=3)
			{
				transformed_indices[i] = triMesh.getIndices()[i];
				transformed_indices[i+1] = triMesh.getIndices()[i+2];
				transformed_indices[i+2] = triMesh.getIndices()[i+1];				
			}
			createBuffer(&transformed_indices[0], mObj->mNumIndices);
		}
		else
		{
			createBuffer(&triMesh.getIndices()[0], mObj->mNumIndices);
		}		
	}
}

void VboMesh::bind( D3D_PRIMITIVE_TOPOLOGY Topology /*= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST*/ ) const
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