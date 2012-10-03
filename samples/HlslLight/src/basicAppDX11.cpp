#include "cinder/app/AppBasic.h"
#include "cinder/Utilities.h"
#include "dx11/RendererDx11.h"
#include "dx11/dx11.h"
#include "dx11/VertexTypes.h"
#include "dx11/HlslEffect.h"
#include "dx11/SimpleVertexBuffer.h"

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
			VertexBufferPC.createInputLayout(effect);
        }

        {// Create vertex buffer
            dx11::VertexPC vertices[] =
            {
                dx11::VertexPC(Vec3f( 0.0f, 0.5f, 0.0f ),ColorA(1,0,0,1)),
                dx11::VertexPC(Vec3f( 0.5f, -0.5f, 0.0f ),ColorA( 0,1,0,1)),
                dx11::VertexPC(Vec3f( -0.5f, -0.5f, 0.0f),ColorA(0,0,1,1)),
            };
			VertexBufferPC.createBuffer(vertices, ARRAYSIZE(vertices));
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

		VertexBufferPC.bind();

		Matrix44f W = Matrix44f::identity();
        Matrix44f V = mCam.getModelViewMatrix();
        Matrix44f P = mCam.getProjectionMatrix();
        Matrix44f MVP = P*V*W;
		effect.uniform("gWorldViewProj", MVP);

		effect.draw(3, 0);
    }

    void resize( ResizeEvent event )
    {
		mCam.lookAt( Vec3f( 0.0f, 0.0f, 10.0f ), Vec3f::zero());
        mCam.setPerspective( 90, event.getAspectRatio(), 0.01f, 1000 );
    }

private: 
	dx11::CameraPerspDX	mCam;

	dx11::SimpleVertexBuffer<dx11::VertexPC>	VertexBufferPC;

	dx11::HlslEffect effect;
};

CINDER_APP_BASIC( BasicApp, RendererDX11)