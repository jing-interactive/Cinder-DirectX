#include "dx11/dx11.h"

#pragma comment( lib, "dxerr.lib" )
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dcompiler.lib" )
#pragma comment( lib, "dxguid.lib" )

#include "cinder/CinderMath.h"
#include "cinder/Vector.h"
#include "cinder/Camera.h"
#include "cinder/TriMesh.h"
#include "cinder/Sphere.h"
#include "cinder/Text.h"
#include "cinder/PolyLine.h"
#include "cinder/Path2d.h"
#include "cinder/Shape2d.h"
#include "cinder/Triangulate.h"
#include "cinder/app/App.h"

#include "dx11/Vbo.h"

namespace{
HRESULT hr = S_OK;
}

namespace cinder { namespace dx11 {

ID3D11Device* g_device;
ID3D11DeviceContext* g_immediateContex;
ID3D11RenderTargetView* g_RenderTargetView;
ID3D11DepthStencilView* g_DepthStencilView;
ID3D11ShaderResourceView* g_DepthSRV;
DXGI_FORMAT g_backBufferFormat;

ID3D11Device* getDevice()
{
    return g_device;
}

ID3D11DeviceContext* getImmediateContext()
{
    return g_immediateContex;
}

ID3D11RenderTargetView* getMainRTV()
{
	return g_RenderTargetView;
}

ID3D11DepthStencilView* getMainDSV()
{
	return g_DepthStencilView;
}

ID3D11ShaderResourceView* getMainDepthSRV()
{
	return g_DepthSRV;
}

DXGI_FORMAT getBackBufferFormat()
{
	return g_backBufferFormat;
}

void setDebugObjectName(_In_ ID3D11DeviceChild* resource, _In_z_ const char *name)
{
  #if defined(_DEBUG)
     resource->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name) - 1, name);
  #endif
}

void clear( const ColorA &color, bool clearDepthBuffer, float clearZ)
{
	g_immediateContex->ClearRenderTargetView(g_RenderTargetView, reinterpret_cast<const float*>(&color));

    if (clearDepthBuffer)
        g_immediateContex->ClearDepthStencilView(g_DepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, clearZ, 0);
}


//--------------------------------------------------------------------------------------
// Use this until D3DX11 comes online and we get some compilation helpers
//--------------------------------------------------------------------------------------
static const unsigned int MAX_INCLUDES = 9;
struct sInclude
{
	HANDLE         hFile;
	HANDLE         hFileMap;
	LARGE_INTEGER  FileSize;
	void           *pMapData;
};

class CIncludeHandler : public ID3DInclude
{
private:
	struct sInclude   m_includeFiles[MAX_INCLUDES];
	unsigned int      m_nIncludes;
	std::vector<DataSourceRef>	m_dataSourceRefs;

public:
	CIncludeHandler()
	{
		// array initialization
		for ( unsigned int i=0; i<MAX_INCLUDES; i++)
		{
			m_includeFiles[i].hFile = INVALID_HANDLE_VALUE;
			m_includeFiles[i].hFileMap = INVALID_HANDLE_VALUE;
			m_includeFiles[i].pMapData = NULL;
		}
		m_nIncludes = 0;
	}
	~CIncludeHandler()
	{
		for (unsigned int i=0; i<m_nIncludes; i++)
		{
			UnmapViewOfFile( m_includeFiles[i].pMapData );

			if ( m_includeFiles[i].hFileMap != INVALID_HANDLE_VALUE)
				CloseHandle( m_includeFiles[i].hFileMap );

			if ( m_includeFiles[i].hFile != INVALID_HANDLE_VALUE)
				CloseHandle( m_includeFiles[i].hFile );
		}

		m_nIncludes = 0;
	}

	STDMETHOD(Open(
		D3D_INCLUDE_TYPE IncludeType,
		LPCSTR pFileName,
		LPCVOID pParentData,
		LPCVOID *ppData,
		UINT *pBytes
		))
	{
#if 1
		DataSourceRef dataSource = app::loadAsset(pFileName);
		if (dataSource->getBuffer().getDataSize() > 0)
		{
			*ppData = dataSource->getBuffer().getData();
			*pBytes = dataSource->getBuffer().getDataSize();
			m_dataSourceRefs.push_back(dataSource);
		}
		else
		{
			return E_FAIL;
		}
#else
		unsigned int   incIndex = m_nIncludes+1;

		// Make sure we have enough room for this include file
		if ( incIndex >= MAX_INCLUDES )
			return E_FAIL;

		// try to open the file
		m_includeFiles[incIndex].hFile  = CreateFileA( pFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_FLAG_SEQUENTIAL_SCAN, NULL );
		if( INVALID_HANDLE_VALUE == m_includeFiles[incIndex].hFile )
		{
			return E_FAIL;
		}

		// Get the file size
		GetFileSizeEx( m_includeFiles[incIndex].hFile, &m_includeFiles[incIndex].FileSize );

		// Use Memory Mapped File I/O for the header data
		m_includeFiles[incIndex].hFileMap = CreateFileMappingA( m_includeFiles[incIndex].hFile, NULL, PAGE_READONLY, m_includeFiles[incIndex].FileSize.HighPart, m_includeFiles[incIndex].FileSize.LowPart, pFileName);
		if( m_includeFiles[incIndex].hFileMap == NULL  )
		{
			if (m_includeFiles[incIndex].hFile != INVALID_HANDLE_VALUE)
				CloseHandle( m_includeFiles[incIndex].hFile );
			return E_FAIL;
		}

		// Create Map view
		*ppData = MapViewOfFile( m_includeFiles[incIndex].hFileMap, FILE_MAP_READ, 0, 0, 0 );
		*pBytes = m_includeFiles[incIndex].FileSize.LowPart;

		// Success - Increment the include file count
		m_nIncludes= incIndex;
#endif
		return S_OK;
	}

	STDMETHOD(Close( LPCVOID pData ))
	{
		// Defer Closure until the container destructor 
		return S_OK;
	}
};

HRESULT compileShader( const Buffer& data, const std::string& entryName, const std::string& shaderModel, ID3DBlob** ppShaderBytecode )
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	//   Instructs the compiler to skip optimization steps during code generation.
	//   Unless you are trying to isolate a problem in your code using this option 
	//   is not recommended.
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;

	//   Hint compiler to prefer flow-control constructs where possible.
	dwShaderFlags |= D3DCOMPILE_PREFER_FLOW_CONTROL;
#endif

	// compiling
	CComPtr<ID3DBlob> pErrorBlob;

	CIncludeHandler includeHandler;

	hr = D3DCompile( data.getData(), data.getDataSize(), NULL,
		NULL, &includeHandler,
		entryName.c_str(), shaderModel.c_str(), 
		dwShaderFlags, 0, 
		ppShaderBytecode, &pErrorBlob);
	if( FAILED(hr) )
	{
		if( pErrorBlob != NULL )
			OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
	}

	return hr;
}

void setModelView( const Camera &cam )
{
    //g_immediateContex->SetTransform(D3DTS_VIEW, (D3DMATRIX*)cam.getModelViewMatrix().m);
}

void setProjection( const Camera &cam )
{
    //g_immediateContex->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)cam.getProjectionMatrix().m);
}

void setMatrices( const Camera &cam )
{
	setProjection( cam );
	setModelView( cam );
}

void enableWireframe()
{
    //g_immediateContex->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void disableWireframe()
{
    //g_immediateContex->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
}

void disableDepthRead()
{
	//g_immediateContex->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
}

void enableDepthRead( bool enable )
{
	//if( enable )
	//	//g_immediateContex->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	//else
	//	//g_immediateContex->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
}

void enableDepthWrite( bool enable )
{
	//if( enable )
	//	//g_immediateContex->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	//else
	//	//g_immediateContex->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
}

void disableDepthWrite()
{
	//g_immediateContex->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
}

void blendFunction(D3D11_BLEND  src, D3D11_BLEND dst)
{
    //g_immediateContex->OMSetBlendState();
    //g_immediateContex->SetRenderState(D3DRS_SRCBLEND, src);
    //g_immediateContex->SetRenderState(D3DRS_DESTBLEND, dst);
}

void enableAlphaBlending( bool premultiplied )
{
	//g_immediateContex->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	if( ! premultiplied )
    {
        blendFunction(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA);
    }
	else
    {
		blendFunction(D3D11_BLEND_ONE , D3D11_BLEND_INV_SRC_ALPHA );
    }
}

void disableAlphaBlending()
{
	//g_immediateContex->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void enableAdditiveBlending()
{
	//g_immediateContex->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    blendFunction(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_ONE );
}

void enableAlphaTest( DWORD value, D3D11_COMPARISON_FUNC func )
{
	//g_immediateContex->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    //g_immediateContex->SetRenderState(D3DRS_ALPHAREF, value);
    //g_immediateContex->SetRenderState(D3DRS_ALPHAFUNC, func);
}

void disableAlphaTest()
{
	//g_immediateContex->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}


void CameraPerspDX::calcProjection()
{	
	mFrustumTop		=  mNearClip * math<float>::tan( (float)M_PI / 180.0f * mFov * 0.5f );
	mFrustumBottom	= -mFrustumTop;
	mFrustumRight	=  mFrustumTop * mAspectRatio;
	mFrustumLeft	= -mFrustumRight;

	float *m = mProjectionMatrix.m;
	m[ 0] =  2.0f * mNearClip / ( mFrustumRight - mFrustumLeft );
	m[ 4] =  0.0f;
	m[ 8] =  ( mFrustumRight + mFrustumLeft ) / ( mFrustumRight - mFrustumLeft );
	m[12] =  0.0f;

	m[ 1] =  0.0f;
	m[ 5] =  2.0f * mNearClip / ( mFrustumTop - mFrustumBottom );
	m[ 9] =  ( mFrustumTop + mFrustumBottom ) / ( mFrustumTop - mFrustumBottom );
	m[13] =  0.0f;

	m[ 2] =  0.0f;
	m[ 6] =  0.0f;
	m[10] = - mFarClip / ( mFarClip - mNearClip);
	m[14] = - mFarClip * mNearClip / ( mFarClip - mNearClip );

	m[ 3] =  0.0f;
	m[ 7] =  0.0f;
	m[11] = -1.0f;
	m[15] =  0.0f;	 
}

void draw( const VboMesh &vbo )
{
	vbo.bind();
	//if (g_currentEffect != NULL)
	//{
	//	if (vbo.getNumIndices() > 0)
	//		g_currentEffect->drawIndexed(vbo.getNumIndices());
	//	else
	//		g_currentEffect->draw(vbo.getNumVertices());
	//}
}

} } // namespace cinder::dx11