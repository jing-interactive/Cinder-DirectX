#include "cinder/app/AppBasic.h"
#include "cinder/Utilities.h"
#include "dx11/RendererDx11.h"
#include "dx11/VertexTypes.h"
#include "dx11/Vbo.h"
#include "dx11/LightHelper.h"
#include "dx11/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Arcball.h"

using namespace ci;
using namespace ci::app; 
using namespace std;

HRESULT hr = S_OK;

class BasicApp : public AppBasic {

public:
    void setup()
    {
		effect = dx11::HlslEffect(loadAsset(L"Lighting.fx"));

		duck.read(loadAsset("ducky.msh"));
		vboDuck = dx11::VboMesh(duck);
		vboDuck.createInputLayout(effect);

		texDuck = dx11::Texture(loadImage(loadAsset("ducky.png")));

		// Directional light.
		mDirLight.Ambient  = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		mDirLight.Diffuse  = Vec4f(0.2f, 0.2f, 0.2f, 1.0f);
		mDirLight.Specular = Vec4f(0.2f, 0.2f, 0.2f, 1.0f);
		mDirLight.Direction = Vec3f(0.57735f, -0.57735f, 0.57735f);

		// Point light--position is changed every frame to animate in UpdateScene function.
		mPointLight.Ambient  = Vec4f(0.3f, 0.3f, 0.3f, 1.0f);
		mPointLight.Diffuse  = Vec4f(0.7f, 0.7f, 0.7f, 1.0f);
		mPointLight.Specular = Vec4f(0.7f, 0.7f, 0.7f, 1.0f);
		mPointLight.Att      = Vec3f(0.0f, 0.1f, 0.0f);
		mPointLight.Range    = 525.0f;

		// Spot light--position and direction changed every frame to animate in UpdateScene function.
		mSpotLight.Ambient  = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		mSpotLight.Diffuse  = Vec4f(1.0f, 1.0f, 0.0f, 1.0f);
		mSpotLight.Specular = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
		mSpotLight.Att      = Vec3f(1.0f, 0.0f, 0.0f);
		mSpotLight.Spot     = 96.0f;
		mSpotLight.Range    = 10000.0f;

		mtrlDuck.Diffuse = Vec4f(0.48f, 0.77f, 0.46f, 1.0f);
		mtrlDuck.Ambient = Vec4f(0.48f, 0.77f, 0.46f, 1.0f);
		mtrlDuck.Specular = Vec4f(0.2f, 0.2f, 0.2f, 16.0f);

		mTransform.setToIdentity();
    }

	void keyDown( KeyEvent event )
	{
		if (event.getCode() == KeyEvent::KEY_ESCAPE)
			quit();
	}

	void mouseDown( MouseEvent event )
	{
		mArcball.mouseDown( event.getPos() );
	}

	void mouseDrag( MouseEvent event )
	{
		mArcball.mouseDrag( event.getPos() );
	}

	void update()
	{
		// animate our little ducky
		mTransform.setToIdentity();
		mTransform.scale(3);
		Matrix44f rot = mArcball.getQuat().toMatrix44();
		mTransform *= rot;
// 		mTransform.rotate( Vec3f::xAxis(), sinf( (float) getElapsedSeconds() * 3.0f ) * 0.08f );
// 		mTransform.rotate( Vec3f::yAxis(), (float) getElapsedSeconds() * 0.5f );
// 		mTransform.rotate( Vec3f::zAxis(), sinf( (float) getElapsedSeconds() * 4.3f ) * 0.09f );

		mPointLight.Position.x = 200.0f*cosf( 0.2f*getElapsedSeconds() );
		mPointLight.Position.z = 200.0f*sinf( 0.2f*getElapsedSeconds() );
// 		mPointLight.Position.y = MathHelper::Max(GetHillHeight(mPointLight.Position.x, 
// 		mPointLight.Position.z), -3.0f) + 10.0f;
		// Set per frame constants.
// 		mDirLight.Direction.x = 70.0f*cosf( 0.2f*getElapsedSeconds() );
// 		mDirLight.Direction.z = 70.0f*sinf( 0.2f*getElapsedSeconds() );
		effect.uniform("gDirLight", &mDirLight);
		effect.uniform("gPointLight", &mPointLight);
		effect.uniform("gSpotLight", &mSpotLight);
		effect.uniform("gEyePosW", mCam.getEyePoint());
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
		effect.uniform("gMaterial", &mtrlDuck);

		effect.drawIndexed(duck.getNumIndices(), 0);
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
	dx11::PointLight mPointLight;
	dx11::SpotLight mSpotLight;

	Matrix44f mTransform;
	dx11::Material mtrlDuck;
	TriMesh	duck;
	dx11::VboMesh	vboDuck;
	dx11::Texture	texDuck;

	dx11::HlslEffect effect;
};

CINDER_APP_BASIC( BasicApp, RendererDX11)