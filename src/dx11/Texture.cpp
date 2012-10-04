#include "dx11/Vbo.h"
#include "dx11/VertexTypes.h"

namespace cinder { namespace dx11 {

Texture::Texture( ImageSourceRef imageSource ):
mObj( std::shared_ptr<Obj>( new Obj ) )
{
	init(imageSource);
}

void Texture::init( ImageSourceRef imageSource )
{
	mObj->mWidth = imageSource->getWidth();
	mObj->mHeight = imageSource->getHeight();
}

Texture::Obj::~Obj()
{
	SAFE_RELEASE(srv);
}

}}