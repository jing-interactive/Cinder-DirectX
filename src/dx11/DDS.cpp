#include "dx11/DDS.h"
#include <assert.h>
#include <xutility>

//--------------------------------------------------------------------------------------
// Return the BPP for a particular format
//--------------------------------------------------------------------------------------
UINT BitsPerPixel( D3DFORMAT fmt )
{
    UINT fmtU = ( UINT )fmt;
    switch( fmtU )
    {
    case D3DFMT_A32B32G32R32F:
        return 128;

    case D3DFMT_A16B16G16R16:
    case D3DFMT_Q16W16V16U16:
    case D3DFMT_A16B16G16R16F:
    case D3DFMT_G32R32F:
        return 64;

    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
    case D3DFMT_A2B10G10R10:
    case D3DFMT_A8B8G8R8:
    case D3DFMT_X8B8G8R8:
    case D3DFMT_G16R16:
    case D3DFMT_A2R10G10B10:
    case D3DFMT_Q8W8V8U8:
    case D3DFMT_V16U16:
    case D3DFMT_X8L8V8U8:
    case D3DFMT_A2W10V10U10:
    case D3DFMT_D32:
    case D3DFMT_D24S8:
    case D3DFMT_D24X8:
    case D3DFMT_D24X4S4:
    case D3DFMT_D32F_LOCKABLE:
    case D3DFMT_D24FS8:
    case D3DFMT_INDEX32:
    case D3DFMT_G16R16F:
    case D3DFMT_R32F:
        return 32;

    case D3DFMT_R8G8B8:
        return 24;

    case D3DFMT_A4R4G4B4:
    case D3DFMT_X4R4G4B4:
    case D3DFMT_R5G6B5:
    case D3DFMT_L16:
    case D3DFMT_A8L8:
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_A8R3G3B2:
    case D3DFMT_V8U8:
    case D3DFMT_CxV8U8:
    case D3DFMT_L6V5U5:
    case D3DFMT_G8R8_G8B8:
    case D3DFMT_R8G8_B8G8:
    case D3DFMT_D16_LOCKABLE:
    case D3DFMT_D15S1:
    case D3DFMT_D16:
    case D3DFMT_INDEX16:
    case D3DFMT_R16F:
    case D3DFMT_YUY2:
        return 16;

    case D3DFMT_R3G3B2:
    case D3DFMT_A8:
    case D3DFMT_A8P8:
    case D3DFMT_P8:
    case D3DFMT_L8:
    case D3DFMT_A4L4:
        return 8;

    case D3DFMT_DXT1:
        return 4;

    case D3DFMT_DXT2:
    case D3DFMT_DXT3:
    case D3DFMT_DXT4:
    case D3DFMT_DXT5:
        return  8;

        // From DX docs, reference/d3d/enums/d3dformat.asp
        // (note how it says that D3DFMT_R8G8_B8G8 is "A 16-bit packed RGB format analogous to UYVY (U0Y0, V0Y1, U2Y2, and so on)")
    case D3DFMT_UYVY:
        return 16;

        // http://msdn.microsoft.com/library/default.asp?url=/library/en-us/directshow/htm/directxvideoaccelerationdxvavideosubtypes.asp
    case MAKEFOURCC( 'A', 'I', '4', '4' ):
    case MAKEFOURCC( 'I', 'A', '4', '4' ):
        return 8;

    case MAKEFOURCC( 'Y', 'V', '1', '2' ):
        return 12;

#if !defined(D3D_DISABLE_9EX)
    case D3DFMT_D32_LOCKABLE:
        return 32;

    case D3DFMT_S8_LOCKABLE:
        return 8;

    case D3DFMT_A1:
        return 1;
#endif // !D3D_DISABLE_9EX

    default:
        assert( FALSE ); // unhandled format
        return 0;
    }
}

UINT BitsPerPixel( DXGI_FORMAT fmt )
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


//--------------------------------------------------------------------------------------
// Get surface information for a particular format
//--------------------------------------------------------------------------------------
void GetSurfaceInfo( UINT width, UINT height, D3DFORMAT fmt, UINT* pNumBytes, UINT* pRowBytes, UINT* pNumRows )
{
    UINT numBytes = 0;
    UINT rowBytes = 0;
    UINT numRows = 0;

    // From the DXSDK docs:
    //
    //     When computing DXTn compressed sizes for non-square textures, the 
    //     following formula should be used at each mipmap level:
    //
    //         max(1, width ?4) x max(1, height ?4) x 8(DXT1) or 16(DXT2-5)
    //
    //     The pitch for DXTn formats is different from what was returned in 
    //     Microsoft DirectX 7.0. It now refers the pitch of a row of blocks. 
    //     For example, if you have a width of 16, then you will have a pitch 
    //     of four blocks (4*8 for DXT1, 4*16 for DXT2-5.)"

    if( fmt == D3DFMT_DXT1 || fmt == D3DFMT_DXT2 || fmt == D3DFMT_DXT3 || fmt == D3DFMT_DXT4 || fmt == D3DFMT_DXT5 )
    {
        int numBlocksWide = 0;
        if( width > 0 )
            numBlocksWide = std::max<int>( 1, width / 4 );
        int numBlocksHigh = 0;
        if( height > 0 )
            numBlocksHigh = std::max<int>( 1, height / 4 );
        int numBytesPerBlock = ( fmt == D3DFMT_DXT1 ? 8 : 16 );
        rowBytes = numBlocksWide * numBytesPerBlock;
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

void GetSurfaceInfo( UINT width, UINT height, DXGI_FORMAT fmt, UINT* pNumBytes, UINT* pRowBytes, UINT* pNumRows )
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


//--------------------------------------------------------------------------------------
#define ISBITMASK( r,g,b,a ) ( ddpf.dwRBitMask == r && ddpf.dwGBitMask == g && ddpf.dwBBitMask == b && ddpf.dwABitMask == a )

//--------------------------------------------------------------------------------------
D3DFORMAT GetD3D9Format( const DDS_PIXELFORMAT& ddpf )
{
    if( ddpf.dwFlags & DDS_RGB )
    {
        switch (ddpf.dwRGBBitCount)
        {
        case 32:
            if( ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0xff000000) )
                return D3DFMT_A8R8G8B8;
            if( ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0x00000000) )
                return D3DFMT_X8R8G8B8;
            if( ISBITMASK(0x000000ff,0x0000ff00,0x00ff0000,0xff000000) )
                return D3DFMT_A8B8G8R8;
            if( ISBITMASK(0x000000ff,0x0000ff00,0x00ff0000,0x00000000) )
                return D3DFMT_X8B8G8R8;

            // Note that many common DDS reader/writers swap the
            // the RED/BLUE masks for 10:10:10:2 formats. We assumme
            // below that the 'correct' header mask is being used
            if( ISBITMASK(0x3ff00000,0x000ffc00,0x000003ff,0xc0000000) )
                return D3DFMT_A2R10G10B10;
            if( ISBITMASK(0x000003ff,0x000ffc00,0x3ff00000,0xc0000000) )
                return D3DFMT_A2B10G10R10;

            if( ISBITMASK(0x0000ffff,0xffff0000,0x00000000,0x00000000) )
                return D3DFMT_G16R16;
            if( ISBITMASK(0xffffffff,0x00000000,0x00000000,0x00000000) )
                return D3DFMT_R32F; // D3DX writes this out as a FourCC of 114
            break;

        case 24:
            if( ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0x00000000) )
                return D3DFMT_R8G8B8;
            break;

        case 16:
            if( ISBITMASK(0x0000f800,0x000007e0,0x0000001f,0x00000000) )
                return D3DFMT_R5G6B5;
            if( ISBITMASK(0x00007c00,0x000003e0,0x0000001f,0x00008000) )
                return D3DFMT_A1R5G5B5;
            if( ISBITMASK(0x00007c00,0x000003e0,0x0000001f,0x00000000) )
                return D3DFMT_X1R5G5B5;
            if( ISBITMASK(0x00000f00,0x000000f0,0x0000000f,0x0000f000) )
                return D3DFMT_A4R4G4B4;
            if( ISBITMASK(0x00000f00,0x000000f0,0x0000000f,0x00000000) )
                return D3DFMT_X4R4G4B4;
            if( ISBITMASK(0x000000e0,0x0000001c,0x00000003,0x0000ff00) )
                return D3DFMT_A8R3G3B2;
            break;
        }
    }
    else if( ddpf.dwFlags & DDS_LUMINANCE )
    {
        if( 8 == ddpf.dwRGBBitCount )
        {
            if( ISBITMASK(0x0000000f,0x00000000,0x00000000,0x000000f0) )
                return D3DFMT_A4L4;
            if( ISBITMASK(0x000000ff,0x00000000,0x00000000,0x00000000) )
                return D3DFMT_L8;
        }

        if( 16 == ddpf.dwRGBBitCount )
        {
            if( ISBITMASK(0x0000ffff,0x00000000,0x00000000,0x00000000) )
                return D3DFMT_L16;
            if( ISBITMASK(0x000000ff,0x00000000,0x00000000,0x0000ff00) )
                return D3DFMT_A8L8;
        }
    }
    else if( ddpf.dwFlags & DDS_ALPHA )
    {
        if( 8 == ddpf.dwRGBBitCount )
        {
            return D3DFMT_A8;
        }
    }
    else if( ddpf.dwFlags & DDS_FOURCC )
    {
        if( MAKEFOURCC( 'D', 'X', 'T', '1' ) == ddpf.dwFourCC )
            return D3DFMT_DXT1;
        if( MAKEFOURCC( 'D', 'X', 'T', '2' ) == ddpf.dwFourCC )
            return D3DFMT_DXT2;
        if( MAKEFOURCC( 'D', 'X', 'T', '3' ) == ddpf.dwFourCC )
            return D3DFMT_DXT3;
        if( MAKEFOURCC( 'D', 'X', 'T', '4' ) == ddpf.dwFourCC )
            return D3DFMT_DXT4;
        if( MAKEFOURCC( 'D', 'X', 'T', '5' ) == ddpf.dwFourCC )
            return D3DFMT_DXT5;

        if( MAKEFOURCC( 'R', 'G', 'B', 'G' ) == ddpf.dwFourCC )
            return D3DFMT_R8G8_B8G8;
        if( MAKEFOURCC( 'G', 'R', 'G', 'B' ) == ddpf.dwFourCC )
            return D3DFMT_G8R8_G8B8;

        if( MAKEFOURCC( 'U', 'Y', 'V', 'Y' ) == ddpf.dwFourCC )
            return D3DFMT_UYVY;
        if( MAKEFOURCC( 'Y', 'U', 'Y', '2' ) == ddpf.dwFourCC )
            return D3DFMT_YUY2;

        // Check for D3DFORMAT enums being set here
        switch( ddpf.dwFourCC )
        {
        case D3DFMT_A16B16G16R16:
        case D3DFMT_Q16W16V16U16:
        case D3DFMT_R16F:
        case D3DFMT_G16R16F:
        case D3DFMT_A16B16G16R16F:
        case D3DFMT_R32F:
        case D3DFMT_G32R32F:
        case D3DFMT_A32B32G32R32F:
        case D3DFMT_CxV8U8:
            return (D3DFORMAT)ddpf.dwFourCC;
        }
    }

    return D3DFMT_UNKNOWN;
}

//--------------------------------------------------------------------------------------
DXGI_FORMAT GetDXGIFormat( const DDS_PIXELFORMAT& ddpf )
{
    if( ddpf.dwFlags & DDS_RGB )
    {
        switch (ddpf.dwRGBBitCount)
        {
        case 32:
            // DXGI_FORMAT_B8G8R8A8_UNORM_SRGB & DXGI_FORMAT_B8G8R8X8_UNORM_SRGB should be
            // written using the DX10 extended header instead since these formats require
            // DXGI 1.1
            //
            // This code will use the fallback to swizzle RGB to BGR in memory for standard
            // DDS files which works on 10 and 10.1 devices with WDDM 1.0 drivers
            //
            // NOTE: We don't use DXGI_FORMAT_B8G8R8X8_UNORM or DXGI_FORMAT_B8G8R8X8_UNORM
            // here because they were defined for DXGI 1.0 but were not required for D3D10/10.1

            if( ISBITMASK(0x000000ff,0x0000ff00,0x00ff0000,0xff000000) )
                return DXGI_FORMAT_R8G8B8A8_UNORM;
            if( ISBITMASK(0x000000ff,0x0000ff00,0x00ff0000,0x00000000) )
                return DXGI_FORMAT_R8G8B8A8_UNORM; // No D3DFMT_X8B8G8R8 in DXGI

            // Note that many common DDS reader/writers swap the
            // the RED/BLUE masks for 10:10:10:2 formats. We assumme
            // below that the 'correct' header mask is being used. The
            // more robust solution is to use the 'DX10' header extension and
            // specify the DXGI_FORMAT_R10G10B10A2_UNORM format directly
            if( ISBITMASK(0x000003ff,0x000ffc00,0x3ff00000,0xc0000000) )
                return DXGI_FORMAT_R10G10B10A2_UNORM;

            if( ISBITMASK(0x0000ffff,0xffff0000,0x00000000,0x00000000) )
                return DXGI_FORMAT_R16G16_UNORM;

            if( ISBITMASK(0xffffffff,0x00000000,0x00000000,0x00000000) )
                // Only 32-bit color channel format in D3D9 was R32F
                return DXGI_FORMAT_R32_FLOAT; // D3DX writes this out as a FourCC of 114
            break;

        case 24:
            // No 24bpp DXGI formats
            break;

        case 16:
            // 5:5:5 & 5:6:5 formats are defined for DXGI, but are deprecated for D3D10+

            if( ISBITMASK(0x0000f800,0x000007e0,0x0000001f,0x00000000) )
                return DXGI_FORMAT_B5G6R5_UNORM;
            if( ISBITMASK(0x00007c00,0x000003e0,0x0000001f,0x00008000) )
                return DXGI_FORMAT_B5G5R5A1_UNORM;
            if( ISBITMASK(0x00007c00,0x000003e0,0x0000001f,0x00000000) )
                return DXGI_FORMAT_B5G5R5A1_UNORM; // No D3DFMT_X1R5G5B5 in DXGI

            // No 4bpp or 3:3:2 DXGI formats
            break;
        }
    }
    else if( ddpf.dwFlags & DDS_LUMINANCE )
    {
        if( 8 == ddpf.dwRGBBitCount )
        {
            if( ISBITMASK(0x000000ff,0x00000000,0x00000000,0x00000000) )
                return DXGI_FORMAT_R8_UNORM; // D3DX10/11 writes this out as DX10 extension

            // No 4bpp DXGI formats
        }

        if( 16 == ddpf.dwRGBBitCount )
        {
            if( ISBITMASK(0x0000ffff,0x00000000,0x00000000,0x00000000) )
                return DXGI_FORMAT_R16_UNORM; // D3DX10/11 writes this out as DX10 extension
            if( ISBITMASK(0x000000ff,0x00000000,0x00000000,0x0000ff00) )
                return DXGI_FORMAT_R8G8_UNORM; // D3DX10/11 writes this out as DX10 extension
        }
    }
    else if( ddpf.dwFlags & DDS_ALPHA )
    {
        if( 8 == ddpf.dwRGBBitCount )
        {
            return DXGI_FORMAT_A8_UNORM;
        }
    }
    else if( ddpf.dwFlags & DDS_FOURCC )
    {
        if( MAKEFOURCC( 'D', 'X', 'T', '1' ) == ddpf.dwFourCC )
            return DXGI_FORMAT_BC1_UNORM;
        if( MAKEFOURCC( 'D', 'X', 'T', '3' ) == ddpf.dwFourCC )
            return DXGI_FORMAT_BC2_UNORM;
        if( MAKEFOURCC( 'D', 'X', 'T', '5' ) == ddpf.dwFourCC )
            return DXGI_FORMAT_BC3_UNORM;

        if( MAKEFOURCC( 'B', 'C', '4', 'U' ) == ddpf.dwFourCC )
            return DXGI_FORMAT_BC4_UNORM;
        if( MAKEFOURCC( 'B', 'C', '4', 'S' ) == ddpf.dwFourCC )
            return DXGI_FORMAT_BC4_SNORM;

        if( MAKEFOURCC( 'A', 'T', 'I', '2' ) == ddpf.dwFourCC )
            return DXGI_FORMAT_BC5_UNORM;
        if( MAKEFOURCC( 'B', 'C', '5', 'S' ) == ddpf.dwFourCC )
            return DXGI_FORMAT_BC5_SNORM;

        if( MAKEFOURCC( 'R', 'G', 'B', 'G' ) == ddpf.dwFourCC )
            return DXGI_FORMAT_R8G8_B8G8_UNORM;
        if( MAKEFOURCC( 'G', 'R', 'G', 'B' ) == ddpf.dwFourCC )
            return DXGI_FORMAT_G8R8_G8B8_UNORM;

        // Check for D3DFORMAT enums being set here
        switch( ddpf.dwFourCC )
        {
        case D3DFMT_A16B16G16R16: // 36
            return DXGI_FORMAT_R16G16B16A16_UNORM;

        case D3DFMT_Q16W16V16U16: // 110
            return DXGI_FORMAT_R16G16B16A16_SNORM;

        case D3DFMT_R16F: // 111
            return DXGI_FORMAT_R16_FLOAT;

        case D3DFMT_G16R16F: // 112
            return DXGI_FORMAT_R16G16_FLOAT;

        case D3DFMT_A16B16G16R16F: // 113
            return DXGI_FORMAT_R16G16B16A16_FLOAT;

        case D3DFMT_R32F: // 114
            return DXGI_FORMAT_R32_FLOAT;

        case D3DFMT_G32R32F: // 115
            return DXGI_FORMAT_R32G32_FLOAT;

        case D3DFMT_A32B32G32R32F: // 116
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
    }

    return DXGI_FORMAT_UNKNOWN;
}