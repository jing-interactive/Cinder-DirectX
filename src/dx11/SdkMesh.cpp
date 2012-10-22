#include "./DXUT/DXUTSDKmesh.h"
#include "dx11/dx11.h"
#include "dx11/SdkMesh.h"
#include "dx11/Vbo.h"
#include "cinder/app/App.h"

HRESULT hr = S_OK;

namespace cinder { namespace dx11{

LPCSTR ConvertSemantic( const D3DDECLUSAGE& d3d9Form )
{
	switch( d3d9Form )
	{
	case D3DDECLUSAGE_POSITION:				return "POSITION";
	case D3DDECLUSAGE_BLENDWEIGHT:			return "BLENDWEIGHT";
	case D3DDECLUSAGE_BLENDINDICES:			return "BLENDINDICES";
	case D3DDECLUSAGE_NORMAL:				return "NORMAL";
	case D3DDECLUSAGE_PSIZE:				return "PSIZE";
	case D3DDECLUSAGE_TEXCOORD:				return "TEXCOORD";
	case D3DDECLUSAGE_TANGENT:				return "TANGENT";
	case D3DDECLUSAGE_BINORMAL:				return "BINORMAL";
	case D3DDECLUSAGE_TESSFACTOR:			return "TESSFACTOR";
	case D3DDECLUSAGE_POSITIONT:			return "POSITIONT";
	case D3DDECLUSAGE_COLOR:				return "COLOR";
	case D3DDECLUSAGE_FOG:					return "FOG";
	case D3DDECLUSAGE_DEPTH:				return "DEPTH";
	case D3DDECLUSAGE_SAMPLE:				return "SAMPLE";
	default:								return "UNKNOWN";
	}
}

DXGI_FORMAT	ConvertType( const D3DDECLTYPE& d3d9Form )
{
	switch( d3d9Form )
	{
	case D3DDECLTYPE_FLOAT1:				return DXGI_FORMAT_R32_FLOAT;
	case D3DDECLTYPE_FLOAT2:				return DXGI_FORMAT_R32G32_FLOAT;
	case D3DDECLTYPE_FLOAT3:				return DXGI_FORMAT_R32G32B32_FLOAT;
	case D3DDECLTYPE_FLOAT4:				return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case D3DDECLTYPE_D3DCOLOR:				return DXGI_FORMAT_R8G8B8A8_UNORM;
	case D3DDECLTYPE_UBYTE4:				return DXGI_FORMAT_R8G8B8A8_UINT;
	case D3DDECLTYPE_SHORT2:				return DXGI_FORMAT_R16G16_SINT;
	case D3DDECLTYPE_SHORT4:				return DXGI_FORMAT_R16G16B16A16_SINT;
	case D3DDECLTYPE_UBYTE4N:				return DXGI_FORMAT_R8G8B8A8_UNORM;
	case D3DDECLTYPE_SHORT2N:				return DXGI_FORMAT_R16G16_SNORM;
	case D3DDECLTYPE_SHORT4N:				return DXGI_FORMAT_R16G16B16A16_SNORM;
	case D3DDECLTYPE_USHORT2N:				return DXGI_FORMAT_R16G16_UNORM;
	case D3DDECLTYPE_USHORT4N:				return DXGI_FORMAT_R16G16B16A16_UNORM;
	case D3DDECLTYPE_UDEC3:					return DXGI_FORMAT_R10G10B10A2_UINT;
	case D3DDECLTYPE_DEC3N:					return DXGI_FORMAT_R10G10B10A2_UNORM;
	case D3DDECLTYPE_FLOAT16_2:				return DXGI_FORMAT_R16G16_FLOAT;
	case D3DDECLTYPE_FLOAT16_4:				return DXGI_FORMAT_R16G16B16A16_FLOAT;

	case D3DDECLTYPE_UNUSED:
	default:								return DXGI_FORMAT_UNKNOWN;
	}
}

HRESULT ConvertDeclaration( const D3DVERTEXELEMENT9 original[ MAX_VERTEX_ELEMENTS ], std::vector<D3D11_INPUT_ELEMENT_DESC>& output)
{
	app::console() << std::endl;
	for( UINT i = 0; i < MAX_VERTEX_ELEMENTS; ++i )
	{
		if( original[i].Stream == 0xFF )
			break;
		// Convert this element
		D3D11_INPUT_ELEMENT_DESC item;
		item.SemanticName			= ConvertSemantic( static_cast< D3DDECLUSAGE >( original[i].Usage ) );
		item.SemanticIndex			= original[i].UsageIndex;
		item.AlignedByteOffset		= original[i].Offset;
		item.InputSlot				= original[i].Stream;
		item.InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
		item.InstanceDataStepRate	= 0;
		item.Format				= ConvertType( static_cast< D3DDECLTYPE >( original[i].Type ) );
		
		app::console() << item.SemanticName << std::endl;

		output.push_back(item);
	}

	return S_OK;
}

SdkMesh::SdkMesh( DataSourceRef dataSource, bool includeUVs /*= true */ )
:mSdkMesh(std::shared_ptr<CDXUTSDKMesh>(new CDXUTSDKMesh))
{
	app::console() << dataSource->getFilePath() << std::endl;
	mSdkMesh->Create(getDevice(), dataSource->getFilePath().c_str());
}

void SdkMesh::load( uint32_t iMesh, VboMesh* target ) const
{
	// create VB
	uint32_t iVB = 0;//TODO: more general
	std::vector<D3D11_INPUT_ELEMENT_DESC> dx11_elements;
	D3DVERTEXELEMENT9* dx9_elements = mSdkMesh->GetVertexElements(iMesh, iVB);
	HR(ConvertDeclaration(dx9_elements, dx11_elements));

	void* pVertices = static_cast<void*>(mSdkMesh->GetRawVerticesAt(iVB));
	size_t nVertices = mSdkMesh->GetNumVertices(iMesh, iVB);
	size_t VertexSize = mSdkMesh->GetVertexStride(iMesh, iVB);

	target->createVertexBuffer(pVertices, nVertices,
		&dx11_elements[0], dx11_elements.size(),VertexSize);

	// create IB
	uint32_t iIB = 0;//TODO: more general
	SDKMESH_INDEX_TYPE idxType = mSdkMesh->GetIndexType(iMesh);
	size_t nIndices = mSdkMesh->GetNumIndices(iMesh);
	if (idxType == IT_16BIT)
	{
		//TODO: potential bug here?
		uint16_t* indices = (uint16_t*)mSdkMesh->GetRawIndicesAt(iIB);
		target->createIndexBuffer(indices, nIndices);
	}
	else if (idxType == IT_32BIT)
	{
		uint32_t* indices = (uint32_t*)mSdkMesh->GetRawIndicesAt(iIB);
		target->createIndexBuffer(indices, nIndices);
	}
	else
	{
		assert(0);
	}
}

}}