#include "cinder/app/AppBasic.h"
#include "cinder/Utilities.h"
#include "dx11/RendererDx11.h"
#include "dx11/VertexTypes.h"
#include "dx11/Vbo.h"
#include "dx11/LightHelper.h"
#include "dx11/Texture.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app; 
using namespace std;

HRESULT hr = S_OK;

class BasicApp : public AppBasic {
private:

public:
    void setup()
    {
		effect = dx11::HlslEffect(loadAsset(L"Lighting.fx"));

		duck.read(loadAsset("ducky.msh"));
		vboDuck = dx11::VboMesh(duck);
		vboDuck.createInputLayout(effect);

		texDuck = dx11::Texture(loadImage(loadAsset("ducky.png")));

		mTransform.setToIdentity();
    }

	void mouseWheel( MouseEvent event )
	{
		event.getWheelIncrement();
	}

	void keyDown( KeyEvent event )
	{
		if (event.getCode() == KeyEvent::KEY_ESCAPE)
			quit();
	}

	void update()
	{
		// animate our little ducky
		mTransform.setToIdentity();
		mTransform.scale(3);
		mTransform.rotate( Vec3f::xAxis(), sinf( (float) getElapsedSeconds() * 3.0f ) * 0.08f );
		mTransform.rotate( Vec3f::yAxis(), (float) getElapsedSeconds() * 0.1f );
		mTransform.rotate( Vec3f::zAxis(), sinf( (float) getElapsedSeconds() * 4.3f ) * 0.09f );
	}

    void draw()
    {
        dx11::clear(ColorA(0.5f, 0.5f, 0.5f));

		vboDuck.bind();

		Matrix44f W = mTransform;
        Matrix44f V = mCam.getModelViewMatrix();
        Matrix44f P = mCam.getProjectionMatrix();
        Matrix44f WVP = P*V*W;
		effect.uniform("gWorld", W);
		effect.uniform("gWorldViewProj", WVP);
		effect.uniform("gDiffuseMap", texDuck);

		effect.drawIndexed(duck.getNumIndices(), 0);
    }

    void resize( ResizeEvent event )
    {
		mCam.lookAt( Vec3f( 0.0f, 0.0f, 10.0f ), Vec3f::zero());
        mCam.setPerspective( 90, event.getAspectRatio(), 0.01f, 1000 );
    }

private: 
	dx11::CameraPerspDX	mCam;

	Matrix44f mTransform;
	TriMesh	duck;
	dx11::VboMesh	vboDuck;
	dx11::Texture	texDuck;

	dx11::HlslEffect effect;
};

CINDER_APP_BASIC( BasicApp, RendererDX11)