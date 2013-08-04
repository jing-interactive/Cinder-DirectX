/*
 Copyright (c) 2013, Vinjn Zhang
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

#include "dx11/dx11.h"
#include "dx11/Shader.h"
#include "cinder/app/App.h"

using namespace std;

namespace
{
    HRESULT hr = S_OK;
}

namespace cinder { namespace dx11 {

//////////////////////////////////////////////////////////////////////////
HRESULT Shader::setup(DataSourceRef dataSrc, const std::string& entryName)
{
    if (dataSrc && dataSrc->getBuffer().getDataSize() > 0)
    {
        CComPtr<ID3DBlob> shaderBytecode;
        HR( dx11::compileShader(dataSrc->getBuffer(), entryName, getProfileName(), &shaderBytecode) );
        HR( doCreateShader(shaderBytecode, &mHandle) );
    }
    return hr;
}

void Shader::bind()
{
    doBind(mHandle);
}

void Shader::unbind()
{
    doBind(NULL);
}

void* VertexShader::getBytecode() const
{
    assert(mShaderBytecode);
    return mShaderBytecode->GetBufferPointer();
}

size_t VertexShader::getBytecodeLength() const
{
    assert(mShaderBytecode);
    return mShaderBytecode->GetBufferSize();
}

HRESULT VertexShader::doCreateShader(ID3DBlob* blob, ID3D11DeviceChild** pHandle)
{
    HR( getDevice()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, 
        reinterpret_cast<ID3D11VertexShader**>(pHandle)) );

    mShaderBytecode = blob; // make a local copy

    return hr;
}

void VertexShader::doBind(ID3D11DeviceChild* handle)
{
    getImmediateContext()->VSSetShader(reinterpret_cast<ID3D11VertexShader*>(handle), NULL, 0);
}

void VertexShader::doSetConstBuffer(uint32_t slot, ID3D11Buffer* buffer)
{
    getImmediateContext()->VSSetConstantBuffers(slot, 1, &buffer);
}

const char* VertexShader::getProfileName() const {return "vs_4_0"; }

class PixelShader : public Shader
{
protected:
    HRESULT doCreateShader(ID3DBlob* blob, ID3D11DeviceChild** pHandle)
    {
        return getDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, 
            reinterpret_cast<ID3D11PixelShader**>(pHandle));
    }

    void doBind(ID3D11DeviceChild* handle)
    {
        getImmediateContext()->PSSetShader(reinterpret_cast<ID3D11PixelShader*>(handle), NULL, 0);
    }

    void doSetConstBuffer(uint32_t slot, ID3D11Buffer* buffer)
    {
        getImmediateContext()->PSSetConstantBuffers(slot, 1, &buffer);
    }

    const char* getProfileName() const {return "ps_4_0"; }
};

class GeometryShader : public Shader
{
protected:
    HRESULT doCreateShader(ID3DBlob* blob, ID3D11DeviceChild** pHandle)
    {
        return getDevice()->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, 
            reinterpret_cast<ID3D11GeometryShader**>(pHandle));
    }

    void doBind(ID3D11DeviceChild* handle)
    {
        getImmediateContext()->GSSetShader(reinterpret_cast<ID3D11GeometryShader*>(handle), NULL, 0);
    }

    void doSetConstBuffer(uint32_t slot, ID3D11Buffer* buffer)
    {
        getImmediateContext()->GSSetConstantBuffers(slot, 1, &buffer);
    }

    const char* getProfileName() const {return "gs_4_0"; }
};


ShaderRef Shader::create(ShaderType type, DataSourceRef dataSrc, const std::string& entryName)
{
    ShaderRef shader;

    switch (type)
    {
    case Shader_VS: shader = ShaderRef(new VertexShader);   break;
    case Shader_PS: shader = ShaderRef(new PixelShader);    break;
    case Shader_GS: shader = ShaderRef(new GeometryShader); break;
    default: throw std::runtime_error("unexpected shader type");
    }

    HR( shader->setup(dataSrc, entryName) );

    return shader;
}

} } // namespace cinder::dx11
