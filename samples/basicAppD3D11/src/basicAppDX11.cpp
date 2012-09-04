#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"
#include "dx11/RendererDx11.h"
#include "dx11/dx11.h"
#include <boost/intrusive_ptr.hpp>
#include "dx11/d3dx11effect.h"
#include "dx11/DDSTextureLoader.h"

using namespace ci;
using namespace ci::app; 
using namespace std;

HRESULT hr = S_OK;

struct SimpleVertex
{
    Vec3f Pos;
};


class BasicApp : public AppBasic {
private:
    HRESULT createShaderFromFile(const wchar_t* fileName, const char* entryName, const char* profileName, 
        ID3D11VertexShader** pVertexShader, ID3DBlob** pBlobOut = NULL)
    {
        fs::path path = getAssetPath(fileName);
        if (!path.empty()){
            ID3DBlob* pShaderBlob = NULL;
            V(dx11::compileShaderFromFile(path.c_str(), entryName, profileName, &pShaderBlob ));

            // Create the vertex shader
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

    HRESULT createShaderFromFile(const wchar_t* fileName, const char* entryName, const char* profileName, ID3D11PixelShader** pPixelShader)
    {
        fs::path path = getAssetPath(fileName);
        if (!path.empty()){
            ID3DBlob* pShaderBlob = NULL;
            V(dx11::compileShaderFromFile(path.c_str(), entryName, profileName, &pShaderBlob ));

            // Create the vertex shader
            V(dx11::getDevice()->CreatePixelShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, pPixelShader));
            SAFE_RELEASE(pShaderBlob);
        }
        return hr;
    }

public:
    void setup()
    {
        {// Create Vertex Shader
            ID3D11VertexShader* ptr;

            ID3DBlob* pBlob;
            V(createShaderFromFile(L"Tutorial02.fx", "VS", "vs_4_0", &ptr, &pBlob));
            pVertexShader = boost::intrusive_ptr<ID3D11VertexShader>(ptr, false);

            // Define the input layout
            D3D11_INPUT_ELEMENT_DESC elements[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            };
            UINT numElements = ARRAYSIZE( elements );

            // Create the input layout
            ID3D11InputLayout* layout;
            V(dx11::getDevice()->CreateInputLayout( elements, numElements, pBlob->GetBufferPointer(),
                pBlob->GetBufferSize(), &layout));

            SAFE_RELEASE(pBlob);
            pInputLayout = boost::intrusive_ptr<ID3D11InputLayout>(layout, false);
        }

        {// Create Pixel Shader
            ID3D11PixelShader* ptr;
            V(createShaderFromFile(L"Tutorial02.fx", "PS", "ps_4_0", &ptr));
            pPixelShader = boost::intrusive_ptr<ID3D11PixelShader>(ptr, false);
        }

        {// Create vertex buffer
            SimpleVertex vertices[] =
            {
                Vec3f( 0.0f, 0.5f, 0.5f ),
                Vec3f( 0.5f, -0.5f, 0.5f ),
                Vec3f( -0.5f, -0.5f, 0.5f ),
            };
            CD3D11_BUFFER_DESC bd(
                sizeof( SimpleVertex ) * 3, 
                D3D11_BIND_VERTEX_BUFFER);

            D3D11_SUBRESOURCE_DATA InitData;
            ZeroMemory( &InitData, sizeof(InitData) );
            InitData.pSysMem = vertices;
            ID3D11Buffer* ptr;
            V(dx11::getDevice()->CreateBuffer( &bd, &InitData, &ptr ));

            pVertexBuffer = boost::intrusive_ptr<ID3D11Buffer>(ptr, false);
        }
    }

    void destroy()
    {
    }

    void keyDown( KeyEvent event )
    {
        if( event.getChar() == 'f' )
        {
            destroy();
            setFullScreen( ! isFullScreen() );
            setup();
        }
        if (event.getCode() == KeyEvent::KEY_ESCAPE)
            quit();
    }

    void draw()
    {
        dx11::clear(ColorA(0.5f, 0.5f, 0.5f));
        dx11::getImmediateContext()->IASetInputLayout(pInputLayout.get());

        UINT stride = sizeof( SimpleVertex );
        UINT offset = 0;
        ID3D11Buffer* vb = pVertexBuffer.get();
        dx11::getImmediateContext()->IASetVertexBuffers( 0, 1, &vb, &stride, &offset );

        // Set primitive topology
        dx11::getImmediateContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

        dx11::getImmediateContext()->VSSetShader( pVertexShader.get(), NULL, 0 );
        dx11::getImmediateContext()->PSSetShader( pPixelShader.get(), NULL, 0 );
        dx11::getImmediateContext()->Draw( 3, 0 );
        dx11::getImmediateContext();
    }

    void resize( ResizeEvent event )
    {
        mCam.lookAt( Vec3f( 0.0f, 0.0f, 10.0f ), Vec3f::zero() );
        mCam.setPerspective( 60, getWindowAspectRatio(), 1, 5000 );
    }

private: 
    CameraPersp	mCam;
    boost::intrusive_ptr<ID3D11VertexShader>     pVertexShader;
    boost::intrusive_ptr<ID3D11PixelShader>      pPixelShader;
    boost::intrusive_ptr<ID3D11InputLayout>      pInputLayout;
    boost::intrusive_ptr<ID3D11Buffer>           pVertexBuffer;
};

CINDER_APP_BASIC( BasicApp, RendererDX11)