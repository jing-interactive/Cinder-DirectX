#include "dx11/Texture.h"
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

static UINT BitsPerPixel( DXGI_FORMAT fmt )
{
	switch( fmt )
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return 128;

	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		return 96;

	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		return 64;

	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8X8_TYPELESS:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		return 32;

	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		return 16;

	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
		return 8;

	case DXGI_FORMAT_R1_UNORM:
		return 1;

	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
		return 4;

	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		return 8;

	default:
		assert( FALSE ); // unhandled format
		return 0;
	}
}

static void GetSurfaceInfo( UINT width, UINT height, DXGI_FORMAT fmt, UINT* pNumBytes, UINT* pRowBytes, UINT* pNumRows )
{
	UINT numBytes = 0;
	UINT rowBytes = 0;
	UINT numRows = 0;

	bool bc = true;
	int bcnumBytesPerBlock = 16;
	switch (fmt)
	{
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		bcnumBytesPerBlock = 8;
		break;

	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		break;

	default:
		bc = false;
		break;
	}

	if( bc )
	{
		int numBlocksWide = 0;
		if( width > 0 )
			numBlocksWide = std::max<int>( 1, width / 4 );
		int numBlocksHigh = 0;
		if( height > 0 )
			numBlocksHigh = std::max<int>( 1, height / 4 );
		rowBytes = numBlocksWide * bcnumBytesPerBlock;
		numRows = numBlocksHigh;
	}
	else
	{
		UINT bpp = BitsPerPixel( fmt );
		rowBytes = ( width * bpp + 7 ) / 8; // round up to nearest byte
		numRows = height;
	}
	numBytes = rowBytes * numRows;
	if( pNumBytes != NULL )
		*pNumBytes = numBytes;
	if( pRowBytes != NULL )
		*pRowBytes = rowBytes;
	if( pNumRows != NULL )
		*pNumRows = numRows;
}

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
				channelOrder = ( imageSource->hasAlpha() ) ? ImageIo::RGBA : ImageIo::RGB;
				if( imageSource->getDataType() == ImageIo::UINT8 )
					mObj->mInternalFormat = ( imageSource->hasAlpha() ) ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM;
				else if( imageSource->getDataType() == ImageIo::UINT16 )
					mObj->mInternalFormat = ( imageSource->hasAlpha() ) ? DXGI_FORMAT_R16G16B16A16_UNORM : DXGI_FORMAT_R16G16B16A16_UNORM;
				else if( imageSource->getDataType() == ImageIo::FLOAT32 && supportsTextureFloat )
					mObj->mInternalFormat = ( imageSource->hasAlpha() ) ? DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_R32G32B32_FLOAT;
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

	//read...
	if( imageSource->getDataType() == ImageIo::UINT8 ) {
		shared_ptr<ImageTargetDXTexture<uint8_t> > target = ImageTargetDXTexture<uint8_t>::createRef( this, channelOrder, isGray, imageSource->hasAlpha() );
		imageSource->load( target );
		init(target->getData());
	}
	else if( imageSource->getDataType() == ImageIo::UINT16 ) {
		shared_ptr<ImageTargetDXTexture<uint16_t> > target = ImageTargetDXTexture<uint16_t>::createRef( this, channelOrder, isGray, imageSource->hasAlpha() );
		imageSource->load( target );
		init(target->getData());
	}
	else {
		shared_ptr<ImageTargetDXTexture<float> > target = ImageTargetDXTexture<float>::createRef( this, channelOrder, isGray, imageSource->hasAlpha() );
		imageSource->load( target );
		init(target->getData());
	}
}

HRESULT Texture::init(const void* pBitData)
{
	HRESULT hr = S_OK;
	// Create the texture 
	CD3D11_TEXTURE2D_DESC desc(mObj->mInternalFormat, mObj->mWidth, mObj->mHeight);
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