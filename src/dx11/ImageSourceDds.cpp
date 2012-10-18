/*
Copyright (c) 2010, The Barbarian Group
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that
the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and
the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include "dx11/DDS.h"
#include "dx11/ImageSourceDds.h"
#include "dx11/V.h"

using namespace std;

namespace cinder {

	///////////////////////////////////////////////////////////////////////////////
	// Registrar
	void ImageSourceDds::registerSelf()
	{
		ImageIoRegistrar::SourceCreationFunc sourceFunc = ImageSourceDds::createSourceRef;
		ImageIoRegistrar::registerSourceType( "dds", sourceFunc, 1 );
	}

	///////////////////////////////////////////////////////////////////////////////
	// ImageSourceDds
	ImageSourceDdsRef ImageSourceDds::createRef( DataSourceRef dataSourceRef, ImageSource::Options options )
	{
		return ImageSourceDdsRef( new ImageSourceDds( dataSourceRef, options ) );
	}

	ImageSourceDds::ImageSourceDds( DataSourceRef dataSourceRef, ImageSource::Options /*options*/ )
		: ImageSource(), pHeapData( 0 ), pHeader( 0 ), mData(0), mDataSize(0), mFormat(DXGI_FORMAT_UNKNOWN), mRowBytes(0)
	{
		if( ! loadData(dataSourceRef) || ! processData())
			throw ImageSourceDdsException();		
	}

	// part of this being separated allows for us to play nicely with the setjmp of libpng
	bool ImageSourceDds::loadData(DataSourceRef dataSourceRef)
	{
		bool success = true;

		// open the file
		HANDLE hFile = CreateFile( dataSourceRef->getFilePath().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_FLAG_SEQUENTIAL_SCAN, NULL );
		if( INVALID_HANDLE_VALUE == hFile )
			return HRESULT_FROM_WIN32( GetLastError() );

		// Get the file size
		LARGE_INTEGER FileSize = {0};
		GetFileSizeEx( hFile, &FileSize );

		// File is too big for 32-bit allocation, so reject read
		if( FileSize.HighPart > 0 )
		{
			CloseHandle( hFile );
			return E_FAIL;
		}

		// Need at least enough data to fill the header and magic number to be a valid DDS
		if( FileSize.LowPart < (sizeof(DDS_HEADER)+sizeof(DWORD)) )
		{
			CloseHandle( hFile );
			return E_FAIL;
		}

		// create enough space for the file data
		pHeapData = new BYTE[ FileSize.LowPart ];
		if( !( pHeapData ) )
		{
			CloseHandle( hFile );
			return E_OUTOFMEMORY;
		}

		// read the data in
		DWORD BytesRead = 0;
		if( !ReadFile( hFile, pHeapData, FileSize.LowPart, &BytesRead, NULL ) )
		{
			CloseHandle( hFile );
			SAFE_DELETE_ARRAY( pHeapData );
			return HRESULT_FROM_WIN32( GetLastError() );
		}

		if( BytesRead < FileSize.LowPart )
		{
			CloseHandle( hFile );
			SAFE_DELETE_ARRAY( pHeapData );
			return E_FAIL;
		}

		// DDS files always start with the same magic number ("DDS ")
		DWORD dwMagicNumber = *( DWORD* )( pHeapData );
		if( dwMagicNumber != DDS_MAGIC )
		{
			CloseHandle( hFile );
			SAFE_DELETE_ARRAY( pHeapData );
			return E_FAIL;
		}

		pHeader = reinterpret_cast<DDS_HEADER*>( pHeapData + sizeof( DWORD ) );

		// Verify header to validate DDS file
		if( pHeader->dwSize != sizeof(DDS_HEADER)
			|| pHeader->ddspf.dwSize != sizeof(DDS_PIXELFORMAT) )
		{
			CloseHandle( hFile );
			SAFE_DELETE_ARRAY( pHeapData );
			return E_FAIL;
		}

		// Check for DX10 extension
		bool bDXT10Header = false;
		if ( (pHeader->ddspf.dwFlags & DDS_FOURCC)
			&& (MAKEFOURCC( 'D', 'X', '1', '0' ) == pHeader->ddspf.dwFourCC) )
		{
			// Must be long enough for both headers and magic value
			if( FileSize.LowPart < (sizeof(DDS_HEADER)+sizeof(DWORD)+sizeof(DDS_HEADER_DXT10)) )
			{
				CloseHandle( hFile );
				SAFE_DELETE_ARRAY( pHeapData );
				return E_FAIL;
			}

			bDXT10Header = true;
		}

		// setup the pointers in the process request
		INT offset = sizeof( DWORD ) + sizeof( DDS_HEADER )
			+ (bDXT10Header ? sizeof( DDS_HEADER_DXT10 ) : 0);
		mData = pHeapData + offset;
		mDataSize = FileSize.LowPart - offset;

		CloseHandle( hFile );

		return success;
	}

	ImageSourceDds::~ImageSourceDds()
	{
		SAFE_DELETE_ARRAY(pHeapData);
	}

	void ImageSourceDds::load( ImageTargetRef target )
	{
// 		// get a pointer to the ImageSource function appropriate for handling our data configuration
// 		ImageSource::RowFunc func = setupRowFunc( target );
// 
// 		const uint8_t *data = mData;
// 		for( int32_t row = 0; row < mHeight; ++row ) {
// 			((*this).*func)( target, row, data );
// 			data += mRowBytes;
// 		}
		void* targetDataPtr = target->getRowPointer(0);
		memcpy(targetDataPtr, mData, mDataSize);
	}

	bool ImageSourceDds::processData()
	{
		UINT iMipCount = pHeader->dwMipMapCount;

		D3D11_TEXTURE2D_DESC desc;
		if ((  pHeader->ddspf.dwFlags & DDS_FOURCC )
			&& (MAKEFOURCC( 'D', 'X', '1', '0' ) == pHeader->ddspf.dwFourCC ) )
		{
			DDS_HEADER_DXT10* d3d10ext = (DDS_HEADER_DXT10*)( (char*)pHeader + sizeof(DDS_HEADER) );

			// For now, we only support 2D textures
			if ( d3d10ext->resourceDimension != D3D11_RESOURCE_DIMENSION_TEXTURE2D )
				return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );

			// Bound array sizes (affects the memory usage below)
			if ( d3d10ext->arraySize > D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION )
				return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );

			desc.ArraySize = d3d10ext->arraySize;
			desc.Format = d3d10ext->dxgiFormat;
		}
		else
		{
			desc.ArraySize = 1;
			desc.Format = GetDXGIFormat( pHeader->ddspf );

			if (pHeader->dwCubemapFlags != 0
				|| (pHeader->dwHeaderFlags & DDS_HEADER_FLAGS_VOLUME) )
			{
				// For now only support 2D textures, not cubemaps or volumes
				return E_FAIL;
			}

			if( desc.Format == DXGI_FORMAT_UNKNOWN )
			{
				D3DFORMAT fmt = GetD3D9Format( pHeader->ddspf );

				// Swizzle some RGB to BGR common formats to be DXGI (1.0) supported
				switch( fmt )
				{
				case D3DFMT_X8R8G8B8:
				case D3DFMT_A8R8G8B8:
					{
						desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

						if ( mDataSize >= 3 )
						{
							for( UINT i = 0; i < mDataSize; i += 4 )
							{
								BYTE a = mData[i];
								mData[i] = mData[i + 2];
								mData[i + 2] = a;
							}
						}
					}
					break;

					// Need more room to try to swizzle 24bpp formats
					// Could also try to expand 4bpp or 3:3:2 formats

				default:
					return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
				}
			}
		}

		mFormat = desc.Format;
		setSize( pHeader->dwWidth, pHeader->dwHeight);

		GetSurfaceInfo(pHeader->dwWidth, pHeader->dwHeight, mFormat, NULL, &mRowBytes, NULL);
		setDataType(ImageIo::UINT8 );
		//HACK: ..
		//setDataType(static_cast<DataType>(desc.Format));
		//setDataType( ( pHeader-> == 16 ) ? ImageIo::UINT16 : ImageIo::UINT8 );

		setColorModel( ImageIo::CM_RGB );
		setChannelOrder( static_cast<ChannelOrder>(desc.Format) );

		// 		switch( mFormat ) {
		// 		case DXGI_FORMAT_BC1_UNORM:
		// 			setColorModel( ImageIo::CM_GRAY );
		// 			setChannelOrder( ImageIo::Y );			
		// 			break;
		// 		case DXGI_FORMAT_R8G8B8A8_UNORM:
		// 			setColorModel( ImageIo::CM_RGB );
		// 			setChannelOrder( ImageIo::RGBA );
		// 			break;
		// 		default:
		// 			throw ImageSourceDdsException();
		// 		}

		return true;
	}


} // namespace cinder