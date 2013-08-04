#include "cinder/app/AppBasic.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "cinder/Arcball.h"
#include "cinder/ObjLoader.h"

#include "dx11/RendererDx11.h"
#include "dx11/Vbo.h"
#include "dx11/Light.h"
#include "dx11/Texture.h"
#include "dx11/Shader.h"

using namespace ci;
using namespace ci::app; 
using namespace std;

HRESULT hr = S_OK;

class BasicApp : public AppBasic {

public:
	void setup()
	{
        mVS = dx11::Shader::create(dx11::Shader_VS, loadAsset("Lighting.fx"), "VS");
        mPS = dx11::Shader::create(dx11::Shader_PS, loadAsset("Lighting.fx"), "PS");

		//ObjLoader loader(loadAsset("wc1.obj"));
		//loader.load(&mesh, boost::tribool::true_value);
		mesh.read(loadAsset("ducky.msh"));
		vboMesh = dx11::VboMesh(mesh, true);
		vboMesh.createInputLayout(mVS.get());

		texDiffuse = dx11::Texture(loadImage(loadAsset("ducky.png")));
		texNormal = dx11::Texture(loadImage(loadAsset("wc1_normal.jpg")));
		texSpecular = dx11::Texture(loadImage(loadAsset("wc1_specular.jpg")));

		// Directional light.
		mDirLight.Ambient  = Vec4f(0.2f, 0.2f, 0.2f, 1.0f);
		mDirLight.Diffuse  = Vec4f(0.7f, 0.7f, 0.7f, 1.0f);
		mDirLight.Specular = Vec4f(0.7f, 0.7f, 0.7f, 1.0f);
		mDirLight.Direction = Vec3f(0.57735f, -0.57735f, 0.57735f);

		mtrlDuck.Diffuse = Vec4f(0.48f, 0.48f, 0.46f, 1.0f);
		mtrlDuck.Ambient = Vec4f(0.48f, 0.48f, 0.46f, 1.0f);
		mtrlDuck.Specular = Vec4f(0.3f, 0.3f, 0.3f, 4.0f);

		mTransform.setToIdentity();
	}

	void keyDown( KeyEvent event )
	{
		if (event.getCode() == KeyEvent::KEY_ESCAPE)
			quit();
		//if (event.getCode() == KeyEvent::KEY_1)
		//{
		//	//effect.useTechnique("LightTech");
		//}
		//else if (event.getCode() == KeyEvent::KEY_2)
		//{
		//	//effect.useTechnique("LightTechNormalMap");
		//}
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
		mTransform.scale(Vec3f(1.5f,1.5f,1.5f));
		Matrix44f rot = mArcball.getQuat().toMatrix44();
		mTransform *= rot;

		mDirLight.Direction.x = cosf( getElapsedSeconds() );
		mDirLight.Direction.y = cosf( 2*getElapsedSeconds() );
		mDirLight.Direction.z = sinf( getElapsedSeconds() );
		mDirLight.Direction.normalize();

		//effect.uniform("gDirLight", &mDirLight);
		//effect.uniform("gEyePosW", mCam.getEyePoint());
	}

	void draw()
	{
		dx11::clear(ColorA(0.5f, 0.5f, 0.5f));

		mVS->bind();
        mPS->bind();

		Matrix44f W = mTransform;
		Matrix44f V = mCam.getModelViewMatrix();
		Matrix44f P = mCam.getProjectionMatrix();
		Matrix44f WVP = P*V*W;
		//effect.uniform("gWorld", W);
		//effect.uniform("gWorldViewProj", WVP);
		//effect.uniform("gDiffuseMap", texDiffuse);
		//effect.uniform("gNormalMap", texNormal);
		//effect.uniform("gSpecularMap", texSpecular);
		//effect.uniform("gMaterial", &mtrlDuck);

		dx11::draw(vboMesh);
	}

	void resize( ResizeEvent event )
	{
		mCam.lookAt( Vec3f( 0.0f, 0.0f, 8.0f ), Vec3f::zero());
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
	dx11::Material mtrlDuck;
	TriMesh	mesh;
	dx11::VboMesh	vboMesh;
	dx11::Texture	texDiffuse, texNormal, texSpecular;

    dx11::ShaderRef mVS, mPS;
};

CINDER_APP_BASIC( BasicApp, RendererDX11)