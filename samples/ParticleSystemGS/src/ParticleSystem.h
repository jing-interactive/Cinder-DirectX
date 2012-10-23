//***************************************************************************************
// ParticleSystem.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <string>
#include <vector>
#include <d3d11.h>
#include <boost/noncopyable.hpp>

#include "cinder/DataSource.h"
#include "cinder/Vector.h"

#include "dx11/Texture.h" 
#include "dx11/HlslEffect.h"

namespace cinder {namespace dx11{

class ParticleSystem : private boost::noncopyable
{
public:
	
	ParticleSystem();
	~ParticleSystem();

	void SetEyePos(const Vec3f& eyePosW);
	void SetEmitPos(const Vec3f& emitPosW);
	void SetEmitDir(const Vec3f& emitDirW);

	void Init(DataSourceRef effect, 
		DataSourceRef dataTexture, 
		size_t maxParticles);

	void Init(HlslEffect effect, 
		Texture dataTexture, 
		size_t maxParticles);

	void Reset();
	void Update(float gameTime);
	void Draw(const Camera& cam);

private:
	void BuildVB();
 
private:
 
	size_t mMaxParticles;
	bool mFirstRun;

	float mGameTime;
	float mTimeStep;

	Vec3f mEyePosW;
	Vec3f mEmitPosW;
	Vec3f mEmitDirW;

	HlslEffect mFX;

	ID3D11Buffer* mInitVB;	
	ID3D11Buffer* mDrawVB;
	ID3D11Buffer* mStreamOutVB;

	ID3D11InputLayout* mInputLayout;
 
	Texture mTexure;
	Texture mRandomTex;
};

}}

#endif // PARTICLE_SYSTEM_H