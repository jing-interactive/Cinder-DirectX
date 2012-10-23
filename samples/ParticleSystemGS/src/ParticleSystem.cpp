//***************************************************************************************
// ParticleSystem.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************
#include "ParticleSystem.h"

#include "cinder/Matrix.h"
#include "cinder/ImageIo.h"

#include "dx11/ImageSourceDds.h" 
#include "dx11/dx11.h"

namespace cinder { namespace dx11{

static HRESULT hr = S_OK;

struct Vertex { 
	Vec3f InitialPos; 
	Vec3f InitialVel; 
	Vec2f Size; 
	float Age; 
	uint32_t Type; 
};

ParticleSystem::ParticleSystem()
: mInitVB(0), mDrawVB(0), mStreamOutVB(0)
{
	mFirstRun = true;
	mGameTime = 0.0f;
	mTimeStep = 0.0f;

	mEyePosW  = Vec3f(0.0f, 0.0f, 0.0f);
	mEmitPosW = Vec3f(0.0f, 0.0f, 0.0f);
	mEmitDirW = Vec3f(0.0f, 1.0f, 0.0f);
}

ParticleSystem::~ParticleSystem()
{
	SAFE_RELEASE(mInitVB);
	SAFE_RELEASE(mDrawVB);
	SAFE_RELEASE(mStreamOutVB);
	SAFE_RELEASE(mInputLayout);
}

void ParticleSystem::SetEyePos(const Vec3f& eyePosW)
{
	mEyePosW = eyePosW;
}

void ParticleSystem::SetEmitPos(const Vec3f& emitPosW)
{
	mEmitPosW = emitPosW;
}

void ParticleSystem::SetEmitDir(const Vec3f& emitDirW)
{
	mEmitDirW = emitDirW;
}

void ParticleSystem::Init(DataSourceRef effect, 
						  DataSourceRef texArraySRV,
						  size_t maxParticles)
{
	Init(HlslEffect(effect), Texture(loadImage(texArraySRV)), maxParticles);
}

void ParticleSystem::Init( HlslEffect effect, Texture dataTexture, size_t maxParticles )
{
	mFX = effect;

	mTexure  = dataTexture;

	mMaxParticles = maxParticles;

	mRandomTex = Texture::createRandom1D();

	const D3D11_INPUT_ELEMENT_DESC elements[5] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"AGE",      0, DXGI_FORMAT_R32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TYPE",     0, DXGI_FORMAT_R32_UINT,        0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	HR(mFX.createInputLayout(elements, ARRAYSIZE(elements), &mInputLayout));
	BuildVB();
}

void ParticleSystem::Reset()
{
	mFirstRun = true;
}

void ParticleSystem::Update(float gameTime)
{
	if (mFirstRun)
		mTimeStep = 0;
	else
		mTimeStep = gameTime - mGameTime;
	mGameTime = gameTime; 
}

void ParticleSystem::Draw(const Camera& cam)
{
	Matrix44f VP = cam.getProjectionMatrix() * cam.getModelViewMatrix();

	ID3D11DeviceContext* dc = getImmediateContext();

	//
	// Set constants.
	//
	mFX.uniform("gViewProj", VP);
	mFX.uniform("gGameTime", mGameTime);
	mFX.uniform("gTimeStep", mTimeStep);
	mFX.uniform("gEyePosW", mEyePosW);
	mFX.uniform("gEmitPosW", mEmitPosW);
	mFX.uniform("gEmitDirW", mEmitDirW);
	mFX.uniform("gTexture", mTexure);
	mFX.uniform("gRandomTex", mRandomTex);

	//
	// Set IA stage.
	//
	dc->IASetInputLayout(mInputLayout);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// On the first pass, use the initialization VB.  Otherwise, use
	// the VB that contains the current particle list.
	if( mFirstRun )
		dc->IASetVertexBuffers(0, 1, &mInitVB, &stride, &offset);
	else
		dc->IASetVertexBuffers(0, 1, &mDrawVB, &stride, &offset);

	//
	// Draw the current particle list using stream-out only to update them.  
	// The updated vertices are streamed-out to the target VB. 
	//
	dc->SOSetTargets(1, &mStreamOutVB, &offset);
	mFX.useTechnique("StreamOutTech");
	if (mFirstRun)
	{
		mFX.draw(1);
		mFirstRun = false;
	}
	else
		mFX.drawAuto();

	// done streaming-out--unbind the vertex buffer
	ID3D11Buffer* bufferArray[1] = {0};
	dc->SOSetTargets(1, bufferArray, &offset);

	// ping-pong the vertex buffers
	std::swap(mDrawVB, mStreamOutVB);

	//
	// Draw the updated particle system we just streamed-out. 
	//
	dc->IASetVertexBuffers(0, 1, &mDrawVB, &stride, &offset);

	mFX.useTechnique("DrawTech");
	mFX.drawAuto();
}

void ParticleSystem::BuildVB()
{
	//
	// Create the buffer to kick-off the particle system.
	//

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(Vertex) * 1;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// The initial particle emitter has type 0 and age 0.  The rest
	// of the particle attributes do not apply to an emitter.
	Vertex p;
	ZeroMemory(&p, sizeof(Vertex));
	p.Age  = 0.0f;
	p.Type = 0; 

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &p;

	HR(getDevice()->CreateBuffer(&vbd, &vinitData, &mInitVB));

	//
	// Create the ping-pong buffers for stream-out and drawing.
	//
	vbd.ByteWidth = sizeof(Vertex) * mMaxParticles;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

	HR(getDevice()->CreateBuffer(&vbd, 0, &mDrawVB));
	HR(getDevice()->CreateBuffer(&vbd, 0, &mStreamOutVB));
}
}}