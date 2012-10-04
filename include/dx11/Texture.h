#pragma once

#include <D3D11.h>
#include "cinder/Cinder.h"
#include "cinder/Surface.h"

namespace cinder { namespace dx11 {

class Texture
{
private:
	struct Obj {
		Obj() : mSrv(NULL),mWidth(-1),mHeight(-1),mInternalFormat(DXGI_FORMAT_UNKNOWN){}
		~Obj();
		ID3D11ShaderResourceView* mSrv;
		int mWidth;
		int mHeight;
		DXGI_FORMAT mInternalFormat;
	};

	std::shared_ptr<Obj>	mObj;

public:
	Texture(){}

	Texture( ImageSourceRef imageSource);

protected:
	void	init( ImageSourceRef imageSource);	
public:
	//@{
	//! Emulates shared_ptr-like behavior
	typedef std::shared_ptr<Obj> Texture::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &Texture::mObj; }
	void reset() { mObj.reset(); }
	//@}  
};

}}