#pragma once

#include "dx11/dx11.h"
#include "cinder/Cinder.h"
#include "cinder/Surface.h"

namespace cinder { namespace dx11 {

class Texture
{
private:
	struct Obj {
		Obj() : mSRV(NULL),mWidth(-1),mHeight(-1),mInternalFormat(DXGI_FORMAT_UNKNOWN){}
		~Obj();
		ID3D11ShaderResourceView* mSRV;
		int mWidth;
		int mHeight;
		DXGI_FORMAT mInternalFormat;
	};

	std::shared_ptr<Obj>	mObj;

public:
	struct Format
	{
		Format();
		//! Enables or disables mipmapping. Default is disabled.
		void	enableMipmapping( bool enableMipmapping = true ) { mMipmapping = enableMipmapping; }
		//! Returns whether the texture has mipmapping enabled
		bool	hasMipmapping() const { return mMipmapping; }
	protected:
		bool			mMipmapping;
	};
	Texture(){}

	Texture( ImageSourceRef imageSource, Format format = Format());

	static Texture createRandom1D(size_t texLength = 1024);

	int getWidth() const;
	int getHeight() const;

	operator ID3D11ShaderResourceView*() const { return mObj->mSRV; }

protected:
	void	init( ImageSourceRef imageSource, const Format &format);	

	HRESULT	init(const void* pBitData, const Format &format );

public:
	//@{
	//! Emulates shared_ptr-like behavior
	typedef std::shared_ptr<Obj> Texture::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &Texture::mObj; }
	void reset() { mObj.reset(); }
	//@}  
};

}}