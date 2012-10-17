//--------------------------------------------------------------------------------------
// File: DXUTSDKMesh.h
//
// Disclaimer:  
//   The SDK Mesh format (.sdkmesh) is not a recommended file format for shipping titles.  
//   It was designed to meet the specific needs of the SDK samples.  Any real-world 
//   applications should avoid this file format in favor of a destination format that 
//   meets the specific needs of the application.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef _SDKMESH_
#define _SDKMESH_

#include <d3dx9math.h>
#include <d3dx11.h>

//--------------------------------------------------------------------------------------
// Hard Defines for the various structures
//--------------------------------------------------------------------------------------
#define SDKMESH_FILE_VERSION 101
#define MAX_VERTEX_ELEMENTS 32
#define MAX_VERTEX_STREAMS 16
#define MAX_FRAME_NAME 100
#define MAX_MESH_NAME 100
#define MAX_SUBSET_NAME 100
#define MAX_MATERIAL_NAME 100
#define MAX_TEXTURE_NAME MAX_PATH
#define MAX_MATERIAL_PATH MAX_PATH
#define INVALID_FRAME ((UINT)-1)
#define INVALID_MESH ((UINT)-1)
#define INVALID_MATERIAL ((UINT)-1)
#define INVALID_SUBSET ((UINT)-1)
#define INVALID_ANIMATION_DATA ((UINT)-1)
#define INVALID_SAMPLER_SLOT ((UINT)-1)
#define ERROR_RESOURCE_VALUE 1

template<typename TYPE> BOOL IsErrorResource( TYPE data )
{
    if( ( TYPE )ERROR_RESOURCE_VALUE == data )
        return TRUE;
    return FALSE;
}
//--------------------------------------------------------------------------------------
// Enumerated Types.  These will have mirrors in both D3D9 and D3D11
//--------------------------------------------------------------------------------------
enum SDKMESH_PRIMITIVE_TYPE
{
    PT_TRIANGLE_LIST = 0,
    PT_TRIANGLE_STRIP,
    PT_LINE_LIST,
    PT_LINE_STRIP,
    PT_POINT_LIST,
    PT_TRIANGLE_LIST_ADJ,
    PT_TRIANGLE_STRIP_ADJ,
    PT_LINE_LIST_ADJ,
    PT_LINE_STRIP_ADJ,
    PT_QUAD_PATCH_LIST,
    PT_TRIANGLE_PATCH_LIST,
};

enum SDKMESH_INDEX_TYPE
{
    IT_16BIT = 0,
    IT_32BIT,
};

enum FRAME_TRANSFORM_TYPE
{
    FTT_RELATIVE = 0,
    FTT_ABSOLUTE,		//This is not currently used but is here to support absolute transformations in the future
};

//--------------------------------------------------------------------------------------
// Structures.  Unions with pointers are forced to 64bit.
//--------------------------------------------------------------------------------------
struct SDKMESH_HEADER
{
    //Basic Info and sizes
    UINT Version;
    BYTE IsBigEndian;
    UINT64 HeaderSize;
    UINT64 NonBufferDataSize;
    UINT64 BufferDataSize;

    //Stats
    UINT NumVertexBuffers;
    UINT NumIndexBuffers;
    UINT NumMeshes;
    UINT NumTotalSubsets;
    UINT NumFrames;
    UINT NumMaterials;

    //Offsets to Data
    UINT64 VertexStreamHeadersOffset;
    UINT64 IndexStreamHeadersOffset;
    UINT64 MeshDataOffset;
    UINT64 SubsetDataOffset;
    UINT64 FrameDataOffset;
    UINT64 MaterialDataOffset;
};

struct SDKMESH_VERTEX_BUFFER_HEADER
{
    UINT64 NumVertices;
    UINT64 SizeBytes;
    UINT64 StrideBytes;
    D3DVERTEXELEMENT9 Decl[MAX_VERTEX_ELEMENTS];
    union
    {
        UINT64 DataOffset;				//(This also forces the union to 64bits)
        ID3D11Buffer* pVB;
    };
};

struct SDKMESH_INDEX_BUFFER_HEADER
{
    UINT64 NumIndices;
    UINT64 SizeBytes;
    UINT IndexType;
    union
    {
        UINT64 DataOffset;				//(This also forces the union to 64bits)
        ID3D11Buffer* pIB;
    };
};

struct SDKMESH_MESH
{
    char Name[MAX_MESH_NAME];
    BYTE NumVertexBuffers;
    UINT VertexBuffers[MAX_VERTEX_STREAMS];
    UINT IndexBuffer;
    UINT NumSubsets;
    UINT NumFrameInfluences; //aka bones

    D3DXVECTOR3 BoundingBoxCenter;
    D3DXVECTOR3 BoundingBoxExtents;

    union
    {
        UINT64 SubsetOffset;	//Offset to list of subsets (This also forces the union to 64bits)
        UINT* pSubsets;	    //Pointer to list of subsets
    };
    union
    {
        UINT64 FrameInfluenceOffset;  //Offset to list of frame influences (This also forces the union to 64bits)
        UINT* pFrameInfluences;      //Pointer to list of frame influences
    };
};

struct SDKMESH_SUBSET
{
    char Name[MAX_SUBSET_NAME];
    UINT MaterialID;
    UINT PrimitiveType;
    UINT64 IndexStart;
    UINT64 IndexCount;
    UINT64 VertexStart;
    UINT64 VertexCount;
};

struct SDKMESH_FRAME
{
    char Name[MAX_FRAME_NAME];
    UINT Mesh;
    UINT ParentFrame;
    UINT ChildFrame;
    UINT SiblingFrame;
    D3DXMATRIX Matrix;
    UINT AnimationDataIndex;		//Used to index which set of keyframes transforms this frame
};

struct SDKMESH_MATERIAL
{
    char    Name[MAX_MATERIAL_NAME];

    // Use MaterialInstancePath
    char    MaterialInstancePath[MAX_MATERIAL_PATH];

    // Or fall back to d3d8-type materials
    char    DiffuseTexture[MAX_TEXTURE_NAME];
    char    NormalTexture[MAX_TEXTURE_NAME];
    char    SpecularTexture[MAX_TEXTURE_NAME];

    D3DXVECTOR4 Diffuse;
    D3DXVECTOR4 Ambient;
    D3DXVECTOR4 Specular;
    D3DXVECTOR4 Emissive;
    FLOAT Power;

    union
    {
        UINT64 Force64_1;			//Force the union to 64bits
        ID3D11Texture2D* pDiffuseTexture;
    };
    union
    {
        UINT64 Force64_2;			//Force the union to 64bits
        ID3D11Texture2D* pNormalTexture;
    };
    union
    {
        UINT64 Force64_3;			//Force the union to 64bits
        ID3D11Texture2D* pSpecularTexture;
    };

    union
    {
        UINT64 Force64_4;			//Force the union to 64bits
        ID3D11ShaderResourceView* pDiffuseRV;
    };
    union
    {
        UINT64 Force64_5;		    //Force the union to 64bits
        ID3D11ShaderResourceView* pNormalRV;
    };
    union
    {
        UINT64 Force64_6;			//Force the union to 64bits
        ID3D11ShaderResourceView* pSpecularRV;
    };

};

struct SDKANIMATION_FILE_HEADER
{
    UINT Version;
    BYTE IsBigEndian;
    UINT FrameTransformType;
    UINT NumFrames;
    UINT NumAnimationKeys;
    UINT AnimationFPS;
    UINT64 AnimationDataSize;
    UINT64 AnimationDataOffset;
};

struct SDKANIMATION_DATA
{
    D3DXVECTOR3 Translation;
    D3DXVECTOR4 Orientation;
    D3DXVECTOR3 Scaling;
};

struct SDKANIMATION_FRAME_DATA
{
    char FrameName[MAX_FRAME_NAME];
    union
    {
        UINT64 DataOffset;
        SDKANIMATION_DATA* pAnimationData;
    };
};

//--------------------------------------------------------------------------------------
// AsyncLoading callbacks
//--------------------------------------------------------------------------------------

typedef void ( CALLBACK*LPCREATETEXTUREFROMFILE )( ID3D11Device* pDev, char* szFileName,
                                                     ID3D11ShaderResourceView** ppRV, void* pContext );
typedef void ( CALLBACK*LPCREATEVERTEXBUFFER )( ID3D11Device* pDev, ID3D11Buffer** ppBuffer,
                                                  D3D11_BUFFER_DESC BufferDesc, void* pData, void* pContext );
typedef void ( CALLBACK*LPCREATEINDEXBUFFER )( ID3D11Device* pDev, ID3D11Buffer** ppBuffer,
                                                 D3D11_BUFFER_DESC BufferDesc, void* pData, void* pContext );
struct SDKMESH_CALLBACKS
{
    LPCREATETEXTUREFROMFILE pCreateTextureFromFile;
    LPCREATEVERTEXBUFFER pCreateVertexBuffer;
    LPCREATEINDEXBUFFER pCreateIndexBuffer;
    void* pContext;
};

//--------------------------------------------------------------------------------------
// CDXUTSDKMesh class.  This class reads the sdkmesh file format for use by the samples
//--------------------------------------------------------------------------------------
class CDXUTSDKMesh
{
private:
    UINT m_NumOutstandingResources;
    bool m_bLoading;
    //BYTE*                         m_pBufferData;
    HANDLE m_hFile;
    HANDLE m_hFileMappingObject;
    //CGrowableArray <BYTE*> m_MappedPointers;
    ID3D11Device* m_pDev;
    //ID3D11DeviceContext* m_pDevContext;

protected:
    //These are the pointers to the two chunks of data loaded in from the mesh file
    BYTE* m_pStaticMeshData;
    BYTE* m_pHeapData;
    BYTE* m_pAnimationData;
    BYTE** m_ppVertices;
    BYTE** m_ppIndices;

    //Keep track of the path
    WCHAR                           m_strPathW[MAX_PATH];
    char                            m_strPath[MAX_PATH];

    //General mesh info
    SDKMESH_HEADER* m_pMeshHeader;
    SDKMESH_VERTEX_BUFFER_HEADER* m_pVertexBufferArray;
    SDKMESH_INDEX_BUFFER_HEADER* m_pIndexBufferArray;
    SDKMESH_MESH* m_pMeshArray;
    SDKMESH_SUBSET* m_pSubsetArray;
    SDKMESH_FRAME* m_pFrameArray;
    SDKMESH_MATERIAL* m_pMaterialArray;

    // Adjacency information (not part of the m_pStaticMeshData, so it must be created and destroyed separately )
    SDKMESH_INDEX_BUFFER_HEADER* m_pAdjacencyIndexBufferArray;

    //Animation (TODO: Add ability to load/track multiple animation sets)
    SDKANIMATION_FILE_HEADER* m_pAnimationHeader;
    SDKANIMATION_FRAME_DATA* m_pAnimationFrameData;
    D3DXMATRIX* m_pBindPoseFrameMatrices;
    D3DXMATRIX* m_pTransformedFrameMatrices;
    D3DXMATRIX* m_pWorldPoseFrameMatrices;

protected:
    void                            LoadMaterials( ID3D11Device* pd3dDevice, SDKMESH_MATERIAL* pMaterials,
                                                   UINT NumMaterials, SDKMESH_CALLBACKS* pLoaderCallbacks=NULL );

    HRESULT                         CreateVertexBuffer( ID3D11Device* pd3dDevice,
                                                        SDKMESH_VERTEX_BUFFER_HEADER* pHeader, void* pVertices,
                                                        SDKMESH_CALLBACKS* pLoaderCallbacks=NULL );

    HRESULT                         CreateIndexBuffer( ID3D11Device* pd3dDevice, SDKMESH_INDEX_BUFFER_HEADER* pHeader,
                                                       void* pIndices, SDKMESH_CALLBACKS* pLoaderCallbacks=NULL );

    HRESULT                 CreateFromFile( ID3D11Device* pDev11,
                                                    LPCTSTR szFileName,
                                                    bool bCreateAdjacencyIndices,
                                                    SDKMESH_CALLBACKS* pLoaderCallbacks11 = NULL);

    HRESULT                 CreateFromMemory( ID3D11Device* pDev11,
                                                      BYTE* pData,
                                                      UINT DataBytes,
                                                      bool bCreateAdjacencyIndices,
                                                      bool bCopyStatic,
                                                      SDKMESH_CALLBACKS* pLoaderCallbacks11 = NULL);

    //frame manipulation
    void                            TransformBindPoseFrame( UINT iFrame, D3DXMATRIX* pParentWorld );
    void                            TransformFrame( UINT iFrame, D3DXMATRIX* pParentWorld, double fTime );
    void                            TransformFrameAbsolute( UINT iFrame, double fTime );

    //Direct3D 11 rendering helpers
    void                            RenderMesh( UINT iMesh,
                                                bool bAdjacent,
                                                ID3D11DeviceContext* pd3dDeviceContext,
                                                UINT iDiffuseSlot,
                                                UINT iNormalSlot,
                                                UINT iSpecularSlot );
    void                            RenderFrame( UINT iFrame,
                                                 bool bAdjacent,
                                                 ID3D11DeviceContext* pd3dDeviceContext,
                                                 UINT iDiffuseSlot,
                                                 UINT iNormalSlot,
                                                 UINT iSpecularSlot );

public:
                                    CDXUTSDKMesh();
                            ~CDXUTSDKMesh();

    HRESULT                 Create( ID3D11Device* pDev11, LPCTSTR szFileName, bool bCreateAdjacencyIndices=
                                            false, SDKMESH_CALLBACKS* pLoaderCallbacks=NULL );
    HRESULT                 Create( ID3D11Device* pDev11, BYTE* pData, UINT DataBytes,
                                            bool bCreateAdjacencyIndices=false, bool bCopyStatic=false,
                                            SDKMESH_CALLBACKS* pLoaderCallbacks=NULL );
    HRESULT                 LoadAnimation( WCHAR* szFileName );
    void                    Destroy();

    //Frame manipulation
    void                            TransformBindPose( D3DXMATRIX* pWorld );
    void                            TransformMesh( D3DXMATRIX* pWorld, double fTime );


    //Direct3D 11 Rendering
    void                    Render( ID3D11DeviceContext* pd3dDeviceContext,
                                            UINT iDiffuseSlot = INVALID_SAMPLER_SLOT,
                                            UINT iNormalSlot = INVALID_SAMPLER_SLOT,
                                            UINT iSpecularSlot = INVALID_SAMPLER_SLOT );
    void                    RenderAdjacent( ID3D11DeviceContext* pd3dDeviceContext,
                                                    UINT iDiffuseSlot = INVALID_SAMPLER_SLOT,
                                                    UINT iNormalSlot = INVALID_SAMPLER_SLOT,
                                                    UINT iSpecularSlot = INVALID_SAMPLER_SLOT );
    //Helpers (D3D11 specific)
    static D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveType( SDKMESH_PRIMITIVE_TYPE PrimType );
    DXGI_FORMAT                     GetIBFormat( UINT iMesh );
    ID3D11Buffer* GetVB( UINT iMesh, UINT iVB );
    ID3D11Buffer* GetIB( UINT iMesh );
    SDKMESH_INDEX_TYPE GetIndexType( UINT iMesh ); 

    ID3D11Buffer* GetAdjIB( UINT iMesh );

    //Helpers (general)
    char* GetMeshPathA();
    WCHAR* GetMeshPathW();
    UINT                            GetNumMeshes();
    UINT                            GetNumMaterials();

    UINT                            GetNumVBs();
    UINT                            GetNumIBs();
    ID3D11Buffer* GetVBAt( UINT iVB );
    ID3D11Buffer* GetIBAt( UINT iIB );

	//added by vinjn
	D3DVERTEXELEMENT9* GetVertexElements( UINT iMesh, UINT iVB );

    BYTE* GetRawVerticesAt( UINT iVB );
    BYTE* GetRawIndicesAt( UINT iIB );
    SDKMESH_MATERIAL* GetMaterial( UINT iMaterial );
    SDKMESH_MESH* GetMesh( UINT iMesh );
    UINT                            GetNumSubsets( UINT iMesh );
    SDKMESH_SUBSET* GetSubset( UINT iMesh, UINT iSubset );
    UINT                            GetVertexStride( UINT iMesh, UINT iVB );
    UINT                            GetNumFrames();
    SDKMESH_FRAME*                  GetFrame( UINT iFrame );
    SDKMESH_FRAME*                  FindFrame( char* pszName );
    UINT64                          GetNumVertices( UINT iMesh, UINT iVB );
    UINT64                          GetNumIndices( UINT iMesh );
    D3DXVECTOR3                     GetMeshBBoxCenter( UINT iMesh );
    D3DXVECTOR3                     GetMeshBBoxExtents( UINT iMesh );
    UINT                            GetOutstandingResources();
    UINT                            GetOutstandingBufferResources();
    bool                            CheckLoadDone();
    bool                            IsLoaded();
    bool                            IsLoading();
    void                            SetLoading( bool bLoading );
    BOOL                            HadLoadingError();

    //Animation
    UINT                            GetNumInfluences( UINT iMesh );
    const D3DXMATRIX*               GetMeshInfluenceMatrix( UINT iMesh, UINT iInfluence );
    UINT                            GetAnimationKeyFromTime( double fTime );
    const D3DXMATRIX*               GetWorldMatrix( UINT iFrameIndex );
    const D3DXMATRIX*               GetInfluenceMatrix( UINT iFrameIndex );
    bool                            GetAnimationProperties( UINT* pNumKeys, FLOAT* pFrameTime );
};

#endif

