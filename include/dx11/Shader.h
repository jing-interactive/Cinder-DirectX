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

#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <exception>
#include <map>

#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Matrix.h"
#include "cinder/DataSource.h"

#include "dx11.h"
#include <boost/noncopyable.hpp>

namespace cinder { namespace dx11 {

enum ShaderType
{
    Shader_VS,
    Shader_PS,
    Shader_GS,
};

//! Represents a DX11 Shader. \ImplShared
class Shader;
typedef std::shared_ptr<Shader> ShaderRef;

class Shader : private boost::noncopyable
{
public: 
    static ShaderRef create(ShaderType type, DataSourceRef dataSrc, const std::string& entryName);

    HRESULT setup(DataSourceRef dataSrc, const std::string& entryName);
    void    bind();
    void    unbind();
    void    setConstBuffer(uint32_t slot, ID3D11Buffer* buffer);

protected:
    virtual HRESULT doCreateShader(ID3DBlob* blob, ID3D11DeviceChild** pHandle) = 0;
    virtual void doBind(ID3D11DeviceChild* handle) = 0;
    virtual void doSetConstBuffer(uint32_t slot, ID3D11Buffer* buffer) = 0;
    virtual const char* getProfileName() const = 0;

    CComPtr<ID3D11DeviceChild> mHandle;
};

// VertexShader is exposed for the use of VboMesh::createInputLayout
class VertexShader : public Shader
{
public:
    void* getBytecode() const;
    size_t getBytecodeLength() const;

protected:
    HRESULT doCreateShader(ID3DBlob* blob, ID3D11DeviceChild** pHandle);
    void doBind(ID3D11DeviceChild* handle);
    void doSetConstBuffer(uint32_t slot, ID3D11Buffer* buffer);
    const char* getProfileName() const;

private:
    CComPtr<ID3DBlob> mShaderBytecode;
};
} } // namespace cinder::dx11
