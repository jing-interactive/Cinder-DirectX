#include "dx11/Texture.h"
#include "dx11/DDS.h"
#include "cinder/ImageIo.h"

using namespace std;

namespace cinder { namespace dx11 {

Texture::Texture( ImageSourceRef imageSource ):
mObj( shared_ptr<Obj>( new Obj ) )
{
	init(imageSource);
}


/////////////////////////////////////////////////////////////////////////////////
// ImageTargetDXTexture
template<typename T>
class ImageTargetDXTexture : public ImageTarget {
public:
	static shared_ptr<ImageTargetDXTexture> createRef( const Texture *aTexture, ImageIo::ChannelOrder &aChannelOrder, bool aIsGray, bool aHasAlpha );
	~ImageTargetDXTexture();

	virtual bool	hasAlpha() const { return mHasAlpha; }
	virtual void*	getRowPointer( int32_t row ) { return mData + row * mRowInc; }

	const void*		getData() const { return mData; }

private:
	ImageTargetDXTexture( const Texture *aTexture, ImageIo::ChannelOrder &aChannelOrder, bool aIsGray, bool aHasAlpha );
	const Texture		*mTexture;
	bool				mIsGray;
	bool				mHasAlpha;
	uint8_t				mPixelInc;
	T					*mData;
	int					mRowInc;
};

void Texture::init( ImageSourceRef imageSource )
{
	mObj->mWidth = imageSource->getWidth();
	mObj->mHeight = imageSource->getHeight();

	// Set the internal format based on the image's color space
	ImageIo::ChannelOrder channelOrder;
	bool isGray = false;
	bool supportsTextureFloat = true;//?
	if( true/*format.isAutoInternalFormat()*/ ) {
		switch( imageSource->getColorModel() ) {
			case ImageIo::CM_RGB:
				channelOrder = ( imageSource->hasAlpha() ) ? ImageIo::RGBA : ImageIo::RGBA;
				if( imageSource->getDataType() == ImageIo::UINT8 )
					mObj->mInternalFormat = ( imageSource->hasAlpha() ) ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM;
				else if( imageSource->getDataType() == ImageIo::UINT16 )
					mObj->mInternalFormat = ( imageSource->hasAlpha() ) ? DXGI_FORMAT_R16G16B16A16_UNORM : DXGI_FORMAT_R16G16B16A16_UNORM;
				else if( imageSource->getDataType() == ImageIo::FLOAT32 && supportsTextureFloat )
					mObj->mInternalFormat = ( imageSource->hasAlpha() ) ? DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_R32G32B32A32_FLOAT;
				else
					mObj->mInternalFormat = ( imageSource->hasAlpha() ) ? DXGI_FORMAT_R8G8B8A8_TYPELESS : DXGI_FORMAT_R8G8B8A8_TYPELESS;
				break;
			case ImageIo::CM_GRAY:
				channelOrder = ( imageSource->hasAlpha() ) ? ImageIo::YA : ImageIo::Y;
				isGray = true;
				if( imageSource->getDataType() == ImageIo::UINT8 )
					mObj->mInternalFormat = ( imageSource->hasAlpha() ) ? DXGI_FORMAT_R8G8_UNORM : DXGI_FORMAT_R8_UNORM;
				else if( imageSource->getDataType() == ImageIo::UINT16 )
					mObj->mInternalFormat = ( imageSource->hasAlpha() ) ? DXGI_FORMAT_R16G16_UNORM : DXGI_FORMAT_R16_UNORM;
				else if( imageSource->getDataType() == ImageIo::FLOAT32 && supportsTextureFloat )
					mObj->mInternalFormat = ( imageSource->hasAlpha() ) ? DXGI_FORMAT_R32G32_FLOAT : DXGI_FORMAT_R32_FLOAT;
				else
					mObj->mInternalFormat = ( imageSource->hasAlpha() ) ? DXGI_FORMAT_R8G8_TYPELESS : DXGI_FORMAT_R8_TYPELESS;
				break;

			default:
				throw ImageIoExceptionIllegalColorModel();
				break;
		}
	}
	else {
		//TODO
		//mObj->mInternalFormat = format.mInternalFormat;
	}

	if (imageSource->getChannelOrder() >= ImageIo::CUSTOM)
		mObj->mInternalFormat = static_cast<DXGI_FORMAT>(imageSource->getChannelOrder());
	//read...
	//ImageTargetDXTexture works like a temp medium, only ImageTargetDXTexture::getData() is needed later
	if( imageSource->getDataType() == ImageIo::UINT8 ) {
		shared_ptr<ImageTargetDXTexture<uint8_t> > target = ImageTargetDXTexture<uint8_t>::createRef( this, channelOrder, isGray, true );
		imageSource->load( target );
		init(target->getData());
	}
	else if( imageSource->getDataType() == ImageIo::UINT16 ) {
		shared_ptr<ImageTargetDXTexture<uint16_t> > target = ImageTargetDXTexture<uint16_t>::createRef( this, channelOrder, isGray, true );
		imageSource->load( target );
		init(target->getData());
	}
	else {
		shared_ptr<ImageTargetDXTexture<float> > target = ImageTargetDXTexture<float>::createRef( this, channelOrder, isGray, true );
		imageSource->load( target );
		init(target->getData());
	}
}

HRESULT Texture::init(const void* pBitData)
{
	HRESULT hr = S_OK;
	// Create the texture 
	CD3D11_TEXTURE2D_DESC desc(mObj->mInternalFormat, mObj->mWidth, mObj->mHeight);
	//TODO: supports more fields
	desc.MipLevels = 1;
	desc.ArraySize = 1;

	D3D11_SUBRESOURCE_DATA* pInitData = new D3D11_SUBRESOURCE_DATA[desc.MipLevels * desc.ArraySize];
	if( !pInitData )
		return E_OUTOFMEMORY;

	UINT NumBytes = 0;
	UINT RowBytes = 0;
	UINT NumRows = 0;
	BYTE* pSrcBits = (BYTE*)pBitData;

	UINT index = 0;
	for( UINT j = 0; j < desc.ArraySize; j++ )
	{
		UINT w = mObj->mWidth;
		UINT h = mObj->mHeight;
		for( UINT i = 0; i < desc.MipLevels; i++ )
		{
			GetSurfaceInfo( w, h, desc.Format, &NumBytes, &RowBytes, &NumRows );
			pInitData[index].pSysMem = ( void* )pSrcBits;
			pInitData[index].SysMemPitch = RowBytes;
			++index;

			pSrcBits += NumBytes;
			w = w >> 1;
			h = h >> 1;
			if( w == 0 )
				w = 1;
			if( h == 0 )
				h = 1;
		}
	}

	ID3D11Texture2D* pTex2D = NULL;
	V_RETURN(dx11::getDevice()->CreateTexture2D( &desc, pInitData, &pTex2D ));
#ifdef _DEBUG
//	pTex2D->SetPrivateData( WKPDID_D3DDebugObjectName, sizeof("dx11::Texture::init")-1, "dx11::Texture::init" );
#endif
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory( &SRVDesc, sizeof( SRVDesc ) );
	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = desc.MipLevels;
	V_RETURN(dx11::getDevice()->CreateShaderResourceView( pTex2D, &SRVDesc, &mObj->mSRV));
	SAFE_RELEASE( pTex2D );

	SAFE_DELETE_ARRAY( pInitData );

	return hr;
}

int Texture::getWidth() const
{
	return mObj->mWidth;
}

int Texture::getHeight() const
{
	return mObj->mHeight;
}

Texture::Obj::~Obj()
{
	SAFE_RELEASE(mSRV);
}


/////////////////////////////////////////////////////////////////////////////////
// ImageTargetDXTexture
template<typename T>
shared_ptr<ImageTargetDXTexture<T> > ImageTargetDXTexture<T>::createRef( const Texture *aTexture, ImageIo::ChannelOrder &aChannelOrder, bool aIsGray, bool aHasAlpha )
{
	return shared_ptr<ImageTargetDXTexture<T> >( new ImageTargetDXTexture<T>( aTexture, aChannelOrder, aIsGray, aHasAlpha ) );
}

template<typename T>
ImageTargetDXTexture<T>::ImageTargetDXTexture( const Texture *aTexture, ImageIo::ChannelOrder &aChannelOrder, bool aIsGray, bool aHasAlpha )
: ImageTarget(), mTexture( aTexture ), mIsGray( aIsGray ), mHasAlpha( aHasAlpha )
{
	if( mIsGray )
		mPixelInc = ( mHasAlpha ) ? 2 : 1;
	else
		mPixelInc = ( mHasAlpha ) ? 4 : 3;
	mRowInc = mTexture->getWidth() * mPixelInc;
	// allocate enough room to hold all these pixels
	mData = new T[mTexture->getHeight() * mRowInc];

	if( boost::is_same<T,uint8_t>::value )
		setDataType( ImageIo::UINT8 );
	else if( boost::is_same<T,uint16_t>::value )
		setDataType( ImageIo::UINT16 );
	else if( boost::is_same<T,float>::value )
		setDataType( ImageIo::FLOAT32 );		

	setChannelOrder( aChannelOrder );
	setColorModel( mIsGray ? ImageIo::CM_GRAY : ImageIo::CM_RGB );
}

template<typename T>
ImageTargetDXTexture<T>::~ImageTargetDXTexture()
{
	delete [] mData;
}

}}