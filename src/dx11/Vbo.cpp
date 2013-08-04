#include "cinder/TriMesh.h"

#include "dx11/Vbo.h"
#include "dx11/VertexTypes.h"
#include "dx11/SdkMesh.h"
#include "dx11/Shader.h"

namespace cinder { namespace dx11 {

static std::vector<Vec4f> computeTangent(const TriMesh &triMesh)
{
    size_t NumVertices = triMesh.getNumVertices();
    const std::vector<Vec3f>& positions = triMesh.getVertices();
    std::vector<uint32_t> indices = triMesh.getIndices();
    if (indices.empty())
    {//make fake indices
        indices.resize(NumVertices);
        for (size_t i=0;i<NumVertices;i++)
            indices[i] = i;
    }
    size_t NumTriangles = indices.size()/3;
    std::vector<Vec3f> TSum(NumVertices);
    std::vector<Vec3f> BSum(NumVertices);

    for (size_t i=0;i<NumTriangles;i++)
    {
        uint32_t i0 = indices[i*3+0];
        uint32_t i1 = indices[i*3+1];
        uint32_t i2 = indices[i*3+2];

        const Vec3f& v0 = triMesh.getVertices()[i0];
        const Vec3f& v1 = triMesh.getVertices()[i1];
        const Vec3f& v2 = triMesh.getVertices()[i2];
        const Vec2f& w0 = triMesh.getTexCoords()[i0];
        const Vec2f& w1 = triMesh.getTexCoords()[i1];
        const Vec2f& w2 = triMesh.getTexCoords()[i2];

        Vec3f e0 = v1 - v0;
        Vec3f e1 = v2 - v0;
        Vec2f d0 = w1 - w0;
        Vec2f d1 = w2 - w0;

        float r = 1.0f / (d0.x * d1.y - d1.x * d0.y);
        Vec3f TValue((d1.y * e0.x - d0.y * e1.x) * r, (d1.y * e0.y - d0.y * e1.y) * r,
            (d1.y * e0.z - d0.y * e1.z) * r);
        Vec3f BValue((d0.x * e1.x - d1.x * e0.x) * r, (d0.x * e1.y - d1.x * e0.y) * r,
            (d0.x * e1.z - d1.x * e0.z) * r);

        TSum[i0] += TValue;
        TSum[i1] += TValue;
        TSum[i2] += TValue;
        BSum[i0] += BValue;
        BSum[i1] += BValue;
        BSum[i2] += BValue;
    }

    std::vector<Vec4f> tangents(NumVertices);
    for (size_t a = 0; a < NumVertices; a++)
    {
        const Vec3f& n = triMesh.getNormals()[a];
        const Vec3f& t = TSum[a];
        // Gram-Schmidt orthogonalize.
        tangents[a] = (t - n * n.dot(t)).normalized();
        // Calculate handedness.
        tangents[a].w = (n.cross(t).dot(BSum[a]) < 0.0f) ? -1.0f : 1.0f;
    }
    return tangents;
}

VboMesh::VboMesh( const TriMesh &triMesh, bool normalMap, bool flipOrder ):
mObj( std::shared_ptr<Obj>( new Obj ) )
{
    bool N = triMesh.hasNormals();
    bool C = triMesh.hasColorsRGB();
    bool Ca = triMesh.hasColorsRGBA();
    bool T = triMesh.hasTexCoords();

    mObj->mNumVertices = triMesh.getNumVertices();

    if (normalMap)
    {//
        assert (N && T);
        std::vector<Vec4f> tangents = computeTangent(triMesh);
        std::vector<VertexNMap> vertices(mObj->mNumVertices);
        for (size_t i=0;i<mObj->mNumVertices;i++)
        {
            vertices[i].position = triMesh.getVertices()[i];
            vertices[i].normal = triMesh.getNormals()[i];
            vertices[i].texCoord = triMesh.getTexCoords()[i];
            vertices[i].tangent = tangents[i].xyz();
        }
        createVertexBuffer<VertexNMap>(&vertices[0], mObj->mNumVertices);
    }
    else
    {
        if (!N && (C || Ca) && !T)
        {//PC
            std::vector<VertexPC> vertices(mObj->mNumVertices);
            for (size_t i=0;i<mObj->mNumVertices;i++)
            {
                vertices[i].position = triMesh.getVertices()[i];
                if (C)
                    vertices[i].color = triMesh.getColorsRGB()[i];
                else
                    vertices[i].color = triMesh.getColorsRGBA()[i];
            }
            createVertexBuffer<VertexPC>(&vertices[0], mObj->mNumVertices);
        }

        if (N && (C || Ca) && !T)
        {//PNC
            std::vector<VertexPNC> vertices(mObj->mNumVertices);
            for (size_t i=0;i<mObj->mNumVertices;i++)
            {
                vertices[i].position = triMesh.getVertices()[i];
                vertices[i].normal = triMesh.getNormals()[i];
                if (C)
                    vertices[i].color = triMesh.getColorsRGB()[i];
                else
                    vertices[i].color = triMesh.getColorsRGBA()[i];
            }
            createVertexBuffer<VertexPNC>(&vertices[0], mObj->mNumVertices);
        }

        if (N && !(C || Ca) && T)
        {//PNT
            std::vector<VertexPNT> vertices(mObj->mNumVertices);
            for (size_t i=0;i<mObj->mNumVertices;i++)
            {
                vertices[i].position = triMesh.getVertices()[i];
                vertices[i].normal = triMesh.getNormals()[i];
                vertices[i].texCoord = triMesh.getTexCoords()[i];
            }
            createVertexBuffer<VertexPNT>(&vertices[0], mObj->mNumVertices);
        }

        if (N && (C || Ca) && T)
        {//PNCT
            std::vector<VertexPNCT> vertices(mObj->mNumVertices);
            for (size_t i=0;i<mObj->mNumVertices;i++)
            {
                vertices[i].position = triMesh.getVertices()[i];
                vertices[i].normal = triMesh.getNormals()[i];
                if (C)
                    vertices[i].color = triMesh.getColorsRGB()[i];
                else
                    vertices[i].color = triMesh.getColorsRGBA()[i];
                vertices[i].texCoord = triMesh.getTexCoords()[i];
            }
            createVertexBuffer<VertexPNCT>(&vertices[0], mObj->mNumVertices);
        }

        if (N && !(C || Ca) && T)
        {//PT
            mObj->mNumVertices = triMesh.getNumVertices();
            std::vector<VertexPT> vertices(mObj->mNumVertices);
            for (size_t i=0;i<mObj->mNumVertices;i++)
            {
                vertices[i].position = triMesh.getVertices()[i];
                vertices[i].texCoord = triMesh.getTexCoords()[i];
            }
            createVertexBuffer<VertexPT>(&vertices[0], mObj->mNumVertices);
        }
    }

    //index buffer
    mObj->mNumIndices = triMesh.getNumIndices();
    if (mObj->mNumIndices > 0)
    {
        if (flipOrder)
        {
            std::vector<uint32_t> transformed_indices(mObj->mNumIndices);
            for (size_t i=0;i<mObj->mNumIndices;i+=3)
            {
                transformed_indices[i] = triMesh.getIndices()[i];
                transformed_indices[i+1] = triMesh.getIndices()[i+2];
                transformed_indices[i+2] = triMesh.getIndices()[i+1];				
            }
            createIndexBuffer(&transformed_indices[0], mObj->mNumIndices);
        }
        else
        {
            createIndexBuffer(&triMesh.getIndices()[0], mObj->mNumIndices);
        }		
    }
}

void VboMesh::bind( D3D_PRIMITIVE_TOPOLOGY Topology /*= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST*/ ) const
{
    dx11::getImmediateContext()->IASetInputLayout(mObj->mInputLayout);
    dx11::getImmediateContext()->IASetPrimitiveTopology( Topology );

    UINT stride = mObj->mVertexStride;
    UINT offset = 0;
    ID3D11Buffer* vextexBuffers[] = {mObj->mVertexBuffer};
    dx11::getImmediateContext()->IASetVertexBuffers( 0, _countof(vextexBuffers), vextexBuffers, &stride, &offset );
    dx11::getImmediateContext()->IASetIndexBuffer(mObj->mIndexBuffer, mObj->mIBFormat, 0 );
}

HRESULT VboMesh::createVertexBuffer( const void* pVertices, UINT nVertices, const D3D11_INPUT_ELEMENT_DESC* pElementDescs, UINT NumInputElements, UINT VertexStride)
{
    mObj = std::shared_ptr<Obj>( new Obj );

    mObj->mNumVertices = nVertices;
    for (size_t i=0;i<NumInputElements;i++)
        mObj->InputElementDescs.push_back(pElementDescs[i]);

    mObj->mVertexStride = VertexStride;
    CD3D11_BUFFER_DESC bd(mObj->mVertexStride*nVertices, D3D11_BIND_VERTEX_BUFFER);

    D3D11_SUBRESOURCE_DATA InitData = {0};
    InitData.pSysMem = pVertices;
    return dx11::getDevice()->CreateBuffer( &bd, &InitData, &mObj->mVertexBuffer );
}

HRESULT VboMesh::createInputLayout(dx11::Shader* shader)
{
    assert(!mObj->InputElementDescs.empty() && "call createBuffer() first");
    dx11::VertexShader* vertexShader = dynamic_cast<dx11::VertexShader*>(shader);
    assert(vertexShader && "The input shader should be a vertex shader");

    return getDevice()->CreateInputLayout(&mObj->InputElementDescs[0], mObj->InputElementDescs.size(), 
        vertexShader->getBytecode(), vertexShader->getBytecodeLength(),
        &mObj->mInputLayout);
}

VboMesh::VboMesh( const SdkMesh &sdkMesh, bool normalMap /*= false*/, bool flipOrder /*= true */ )
{
    sdkMesh.load(0, this);
}

}}