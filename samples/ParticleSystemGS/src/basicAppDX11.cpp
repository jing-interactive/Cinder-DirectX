#include "cinder/app/AppBasic.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "cinder/Arcball.h"
#include "cinder/ObjLoader.h"
#include "cinder/Rand.h"

#include "dx11/RendererDx11.h"
#include "dx11/VertexTypes.h"
#include "dx11/Vbo.h"
#include "dx11/LightHelper.h"
#include "dx11/Texture.h"
#include "dx11/ImageSourceDds.h" 

#include "ParticleSystem.h"

#ifdef _DEBUG
#pragma comment(lib, "CinderDirectX_d.lib")
#else
#pragma comment(lib, "CinderDirectX.lib")
#endif // _DEBUG

using namespace ci;
using namespace ci::app; 
using namespace std;

class BasicApp : public AppBasic {

public:
	void prepareSettings( Settings *settings )
	{
		settings->setWindowSize(800, 600);
	}

	void setup()
	{
		addAssetDirectory("c:\\Program Files\\Microsoft DirectX SDK (June 2010)\\Samples\\Media\\");
		addAssetDirectory("c:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Samples\\Media\\");

		mParticleSys.Init(loadAsset("Fire.fx"), loadAsset("flare0.png"), 100);

		dx11::Texture::Format format;
		format.enableMipmapping(true);

		// Directional light.
		mDirLight.Ambient  = Vec4f(0.1f, 0.1f, 0.1f, 1.0f);
		mDirLight.Diffuse  = Vec4f(0.4f, 0.4f, 0.4f, 1.0f);
		mDirLight.Specular = Vec4f(0.2f, 0.2f, 0.2f, 1.0f);
		mDirLight.Direction = Vec3f(0.57735f, -0.57735f, 0.57735f);

		mScale = 2;
		
		mParticleSys.SetEmitPos(Vec3f(0.0f, 0.0f, 0.0f));

		mTransform.setToIdentity();
	}

	void keyDown( KeyEvent event )
	{
		if (event.getCode() == KeyEvent::KEY_ESCAPE)
			quit();
	}

	void mouseWheel( MouseEvent event )
	{
		mScale += event.getWheelIncrement()*0.1f;
		constrain(mScale, 0.5f, 5.0f);
	}

	void mouseDown( MouseEvent event )
	{
		mArcball.mouseDown( Vec2i(event.getPos().x, getWindowHeight()-event.getPos().y) );
	}

	void mouseDrag( MouseEvent event )
	{
		mArcball.mouseDrag( Vec2i(event.getPos().x, getWindowHeight()-event.getPos().y) );
	}

	void update()
	{
		// animate our little ducky
		mTransform.setToIdentity();
		mTransform.scale(Vec3f(mScale, mScale, mScale));
		Matrix44f rot = mArcball.getQuat().toMatrix44();
		mTransform *= rot;

		mDirLight.Direction.x = cosf( getElapsedSeconds() );
		mDirLight.Direction.z = sinf( getElapsedSeconds() );
		mDirLight.Direction.normalize();

		mParticleSys.SetEmitDir(mDirLight.Direction);
		mParticleSys.Update(getElapsedSeconds());
	}

	void draw()
	{
		dx11::clear(ColorA(0.5f, 0.5f, 0.5f));

		Matrix44f W = mTransform;
		Matrix44f V = mCam.getModelViewMatrix();
		Matrix44f P = mCam.getProjectionMatrix();
		Matrix44f WVP = P*V*W;

		mParticleSys.SetEyePos(mCam.getEyePoint());
		mParticleSys.Draw(mCam);
	}

	void resize( ResizeEvent event )
	{
		mCam.lookAt( Vec3f( 0.0f, 0.0f, 10.0f ), Vec3f::zero());
		mCam.setPerspective( 90, event.getAspectRatio(), 0.01f, 1000 );

		mArcball.setWindowSize( getWindowSize() );
		mArcball.setCenter( Vec2f( getWindowWidth() / 2.0f, getWindowHeight() / 2.0f ) );
		mArcball.setRadius( 150 );
	}

private: 
	Arcball	mArcball;
	dx11::CameraPerspDX	mCam;

	dx11::DirectionalLight mDirLight;

	Matrix44f mTransform;
	float	mScale;

	dx11::ParticleSystem mParticleSys;
};

CINDER_APP_BASIC( BasicApp, RendererDX11)