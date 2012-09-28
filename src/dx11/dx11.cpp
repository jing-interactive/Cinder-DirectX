#include "dx11/dx11.h"

#pragma comment( lib, "dxerr.lib" )
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dcompiler.lib" )

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

namespace{
HRESULT hr = S_OK;
}

namespace cinder { namespace dx11 {

ID3D11Device* g_device = NULL;
ID3D11DeviceContext* g_immediateContex = NULL;
ID3D11RenderTargetView* g_RenderTargetView = NULL;
ID3D11DepthStencilView* g_DepthStencilView = NULL;

ID3D11Device* getDevice()
{
    return g_device;
}

ID3D11DeviceContext* getImmediateContext()
{
    return g_immediateContex;
}

void clear( const ColorA &color, bool clearDepthBuffer, float clearZ)
{
	g_immediateContex->ClearRenderTargetView(g_RenderTargetView, reinterpret_cast<const float*>(&color));

    if (clearDepthBuffer)
        g_immediateContex->ClearDepthStencilView(g_DepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, clearZ, 0);
}


HRESULT compileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    // open the file
    HANDLE hFile = CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                               FILE_FLAG_SEQUENTIAL_SCAN, NULL );
    if( INVALID_HANDLE_VALUE == hFile )
        return D3D11_ERROR_FILE_NOT_FOUND;

    // Get the file size
    LARGE_INTEGER FileSize;
    GetFileSizeEx( hFile, &FileSize );

    // create enough space for the file data
    BYTE* pFileData = new BYTE[ FileSize.LowPart ];
    if( !pFileData )
        return E_OUTOFMEMORY;

    // read the data in
    DWORD BytesRead;
    if( !ReadFile( hFile, pFileData, FileSize.LowPart, &BytesRead, NULL ) )
        return E_FAIL; 

    CloseHandle( hFile );

    // compiling
    ID3DBlob* pErrorBlob = NULL;

    hr = D3DCompile( pFileData, FileSize.LowPart, NULL,
        NULL, NULL,
        szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, 
        ppBlobOut, &pErrorBlob);
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
    }
    SAFE_RELEASE(pErrorBlob);

    return hr;
}

HRESULT createShaderFromPath(const fs::path& filePath, const char* entryName, const char* profileName, 
	ID3D11VertexShader** pVertexShader, ID3DBlob** pBlobOut)
{
    HRESULT hr = E_FAIL;
	if (!filePath.empty()){
		ID3DBlob* pShaderBlob = NULL;
		V(dx11::compileShaderFromFile(filePath.c_str(), entryName, profileName, &pShaderBlob ));

		V(dx11::getDevice()->CreateVertexShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, pVertexShader));

		if (pBlobOut)
		{// Add reference
			pShaderBlob->AddRef();
			*pBlobOut = pShaderBlob;
		}
		SAFE_RELEASE(pShaderBlob);
	}
	return hr;
}

HRESULT createShaderFromPath(const fs::path& filePath, const char* entryName, const char* profileName, ID3D11PixelShader** pPixelShader)
{
    HRESULT hr = E_FAIL;
	if (!filePath.empty()){
		ID3DBlob* pShaderBlob = NULL;
		V(dx11::compileShaderFromFile(filePath.c_str(), entryName, profileName, &pShaderBlob ));

		V(dx11::getDevice()->CreatePixelShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(),
			NULL, pPixelShader));
		SAFE_RELEASE(pShaderBlob);
	}
	return hr;
}

HRESULT createEffectFromPath(const fs::path& filePath, ID3DX11Effect** pEffect)
{
    HRESULT hr = E_FAIL;
	if (!filePath.empty()){
		ID3DBlob* pShaderBlob = NULL;
		V(dx11::compileShaderFromFile(filePath.c_str(), "None", "fx_5_0", &pShaderBlob ));

		V(D3DX11CreateEffectFromMemory(pShaderBlob->GetBufferPointer(),pShaderBlob->GetBufferSize(),
			0,	dx11::getDevice(),pEffect));
		SAFE_RELEASE(pShaderBlob);
	}
	return hr;
}

void drawWithTechnique(ID3DX11EffectTechnique* tech, UINT VertexCount, UINT StartVertexLocation)
{
    HRESULT hr = S_OK;
    D3DX11_TECHNIQUE_DESC techDesc;
    tech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        tech->GetPassByIndex(p)->Apply(0, getImmediateContext());
	    getImmediateContext()->Draw(VertexCount, StartVertexLocation);
    }
}

void drawIndexedWithTechnique(ID3DX11EffectTechnique* tech, UINT VertexCount, UINT StartVertexLocation, INT BaseVertexLocation)
{
    HRESULT hr = S_OK;
    D3DX11_TECHNIQUE_DESC techDesc;
    tech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        tech->GetPassByIndex(p)->Apply(0, getImmediateContext());
	    getImmediateContext()->DrawIndexed(VertexCount, StartVertexLocation, BaseVertexLocation);
    }
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

} } // namespace cinder::dx11