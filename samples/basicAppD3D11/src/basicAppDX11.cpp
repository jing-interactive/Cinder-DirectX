#include "cinder/app/AppBasic.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"
#include "dx11/RendererDx11.h"
#include "dx11/dx11.h"
#include <boost/intrusive_ptr.hpp>
#include "dx11/DDSTextureLoader.h"
#include "dx11/VertexTypes.h"
#include "dx11/HlslEffect.h"

using namespace ci;
using namespace ci::app; 
using namespace std;

HRESULT hr = S_OK;

class BasicApp : public AppBasic {
private:

public:
    void setup()
    {
        {// Create Effect
			effect = dx11::HlslEffect(loadAsset(L"color.fx"));

	        // Create the input layout 
            V(effect.createInputLayout("ColorTech", dx11::VertexPC::InputElements, 
				dx11::VertexPC::InputElementCount,	&pInputLayout));
        }

        {// Create vertex buffer
            dx11::VertexPC vertices[] =
            {
                dx11::VertexPC(Vec3f( 0.0f, 0.5f, 0.0f ),ColorA(1,0,0,1)),
                dx11::VertexPC(Vec3f( 0.5f, -0.5f, 0.0f ),ColorA( 0,1,0,1)),
                dx11::VertexPC(Vec3f( -0.5f, -0.5f, 0.0f),ColorA(0,0,1,1)),
            };
            CD3D11_BUFFER_DESC bd(sizeof(vertices), D3D11_BIND_VERTEX_BUFFER);

            D3D11_SUBRESOURCE_DATA InitData = {0};
            InitData.pSysMem = vertices;
            V(dx11::getDevice()->CreateBuffer( &bd, &InitData, &pVertexBuffer ));
        }
    }

    void destroy()
    {
		effect.reset();
        SAFE_RELEASE(pInputLayout);
        SAFE_RELEASE(pVertexBuffer);
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
        dx11::getImmediateContext()->IASetInputLayout(pInputLayout);

        UINT stride = sizeof( dx11::VertexPC);
        UINT offset = 0;
        dx11::getImmediateContext()->IASetVertexBuffers( 0, 1, &pVertexBuffer, &stride, &offset );

        // Set const
		Matrix44f W = Matrix44f::identity();
        Matrix44f V = mCam.getModelViewMatrix();
        Matrix44f P = mCam.getProjectionMatrix();
        Matrix44f MVP = P*V*W;
		effect.uniform("gWorldViewProj", MVP);

        // Set primitive topology
        dx11::getImmediateContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
  //      dx11::drawWithTechnique(mTech, 3, 0);
		effect.draw("ColorTech", 3, 0);
    }

    void resize( ResizeEvent event )
    {
		mCam.lookAt( Vec3f( 0.0f, 0.0f, 10.0f ), Vec3f::zero());
        mCam.setPerspective( 90, event.getAspectRatio(), 0.01f, 1000 );
    }

private: 
	dx11::CameraPerspDX	mCam;
 
    ID3D11InputLayout*      pInputLayout;
    ID3D11Buffer*           pVertexBuffer;

	dx11::HlslEffect effect;
};

CINDER_APP_BASIC( BasicApp, RendererDX11)