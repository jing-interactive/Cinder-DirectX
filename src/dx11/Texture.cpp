#include "dx11/Texture.h"
#include "dx11/DDS.h"
#include "cinder/Rand.h"
#include "cinder/ImageIo.h"

using namespace std;

namespace cinder { namespace dx11 {

/////////////////////////////////////////////////////////////////////////////////
// ImageTargetDXTexture
template<typename T>
class ImageTargetDXTexture : public ImageTarget {
public:
	static std::shared_ptr<ImageTargetDXTexture> createRef( const Texture *aTexture, ImageIo::ChannelOrder &aChannelOrder, bool aIsGray, bool aHasAlpha );
	~ImageTargetDXTexture();

	virtual bool	hasAlpha() const { return mHasAlpha; }
	virtual void*	getRowPointer( int32_t row ) { return mData + row * mRowInc; }

private:
	ImageTargetDXTexture( const Texture *aTexture, ImageIo::ChannelOrder &aChannelOrder, bool aIsGray, bool aHasAlpha );
	const Texture		*mTexture;
	bool				mIsGray;
	bool				mHasAlpha;
	uint8_t				mPixelInc;
	T					*mData;
	int					mRowInc;
};

Texture::Texture( ImageSourceRef imageSource, Format format/* = Format() */):
mObj( shared_ptr<Obj>( new Obj ) )
{	
	if (format.hasMipmapping())
	{
		UINT support = 0;
		HRESULT hr = S_OK;
		HR(dx11::getDevice()->CheckFormatSupport(static_cast<DXGI_FORMAT>(imageSource->getChannelOrder()), &support));
		if (!(support & D3D11_FORMAT_SUPPORT_RENDER_TARGET))
		{
			format.enableMipmapping(false);
		}
	}
	init(imageSource, format);
}

void Texture::init( ImageSourceRef imageSource, const Format &format )
{
	int32_t w = imageSource->getWidth();
	int32_t h = imageSource->getHeight();

	//HACK!!
	if (w > 10000 && h > 100000)
	{
		mObj->mArraySize = w/10000;
		mObj->mWidth = w%10000;

		mObj->mMipLevels = h/100000;
		mObj->mHeight = h%100000;
	}
    else
    {
        mObj->mWidth = w;
        mObj->mHeight = h;
    }

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
		init(target->getRowPointer(0), format);
	}
	else if( imageSource->getDataType() == ImageIo::UINT16 ) {
		shared_ptr<ImageTargetDXTexture<uint16_t> > target = ImageTargetDXTexture<uint16_t>::createRef( this, channelOrder, isGray, true );
		imageSource->load( target );
		init(target->getRowPointer(0), format);
	}
	else {
		shared_ptr<ImageTargetDXTexture<float> > target = ImageTargetDXTexture<float>::createRef( this, channelOrder, isGray, true );
		imageSource->load( target );
		init(target->getRowPointer(0), format);
	}
}

HRESULT Texture::init(const void* pBitData, const Format &format )
{
	HRESULT hr = S_OK;
	// Create the texture 
	CD3D11_TEXTURE2D_DESC desc(mObj->mInternalFormat, mObj->mWidth, mObj->mHeight);
	//TODO: supports more fields
	desc.MipLevels = mObj->mMipLevels;
	desc.ArraySize = mObj->mArraySize;
	if (format.hasMipmapping() && desc.MipLevels == 1)
	{
		desc.BindFlags |=  D3D11_BIND_RENDER_TARGET;
		desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

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

	CD3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc(D3D_SRV_DIMENSION_TEXTURE2D, desc.Format);
	//TODO
	//SRVDesc.Texture2D.MipLevels = desc.MipLevels;
	V_RETURN(dx11::getDevice()->CreateShaderResourceView( pTex2D, &SRVDesc, &mObj->mSRV));

	if (format.hasMipmapping() && desc.MipLevels == 1 )
		dx11::getImmediateContext()->GenerateMips(mObj->mSRV);
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

Texture Texture::createRandom1D(size_t texLength)
{
	HRESULT hr = S_OK;
	// 
	// Create the random data.
	//
	Vec4f* randomValues = new Vec4f[texLength];

	for (size_t i = 0; i < texLength; ++i)
	{
		randomValues[i].x = randFloat(-1.0f, 1.0f);
		randomValues[i].y = randFloat(-1.0f, 1.0f);
		randomValues[i].z = randFloat(-1.0f, 1.0f);
		randomValues[i].w = randFloat(-1.0f, 1.0f);
	}

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = randomValues;
	initData.SysMemPitch = texLength*sizeof(Vec4f);
	initData.SysMemSlicePitch = 0;

	//
	// Create the texture.
	//
	D3D11_TEXTURE1D_DESC texDesc;
	texDesc.Width = texLength;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.ArraySize = 1;

	ID3D11Texture1D* randomTex = 0;
	HR(getDevice()->CreateTexture1D(&texDesc, &initData, &randomTex));

	delete[] randomValues;

	//
	// Create the resource view.
	//
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
	viewDesc.Texture1D.MostDetailedMip = 0;

	ID3D11ShaderResourceView* randomTexSRV = 0;
	getDevice()->CreateShaderResourceView(randomTex, &viewDesc, &randomTexSRV);

	SAFE_RELEASE(randomTex);

	Texture newTex;
	newTex.mObj = shared_ptr<Obj>(new Obj);
	newTex.mObj->mWidth = texLength;
	newTex.mObj->mHeight = 1;
	newTex.mObj->mInternalFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
	newTex.mObj->mSRV = randomTexSRV;

	return newTex;
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
	return shared_ptr<ImageTargetDXTexture<T> >( new ImageTargetDXTexture<T>( aTexture, aChannelOrder, aIsGray, aHasAlpha) );
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


Texture::Format::Format()
{
	mMipmapping = false;
}

}}