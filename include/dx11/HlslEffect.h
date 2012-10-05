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

#include "dx11/d3dx11effect.h"

namespace cinder { namespace dx11 {

HRESULT createEffect(DataSourceRef datasrc, ID3DX11Effect** pEffect);

void drawWithTechnique(ID3DX11EffectTechnique* tech, UINT VertexCount, UINT StartVertexLocation);

void drawIndexedWithTechnique(ID3DX11EffectTechnique* tech, UINT IndexCount, UINT StartVertexLocation, INT BaseVertexLocation);

//! Represents an Hlsl Effect. \ImplShared
class HlslEffect {
  public: 
	HlslEffect() {}
	HlslEffect( DataSourceRef effect);

	void	useTechnique(const std::string &name);

	HRESULT createInputLayout(const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs, 
		UINT NumElements,ID3D11InputLayout **ppInputLayout, int pass = 0);

	void draw(UINT VertexCount, UINT StartVertexLocation = 0);
	void drawIndexed(UINT IndexCount, UINT StartVertexLocation = 0, INT BaseVertexLocation = 0);

	void	uniform( const std::string &name, ID3D11ShaderResourceView* pSRV );

	template <typename T>
	void	uniform( const std::string &name, T *data)
	{
		ID3DX11EffectVariable* var = getVariable( name );
		if (var)
		{
			var->SetRawValue((void*)data, 0, sizeof(T));
		}
	}

	void	uniform( const std::string &name, int data );
	void	uniform( const std::string &name, const Vec2i &data );
	void	uniform( const std::string &name, const int *data, int count );		
	void	uniform( const std::string &name, const Vec2i *data, int count );	
	void	uniform( const std::string &name, float data );
	void	uniform( const std::string &name, const Vec2f &data );
	void	uniform( const std::string &name, const Vec3f &data );
	void	uniform( const std::string &name, const Vec4f &data );
	void	uniform( const std::string &name, const Color &data );
	void	uniform( const std::string &name, const ColorA &data );
	void	uniform( const std::string &name, const Matrix33f &data, bool transpose = false );
	void	uniform( const std::string &name, const Matrix44f &data, bool transpose = false );
	void	uniform( const std::string &name, const float *data, int count );
	void	uniform( const std::string &name, const Vec2f *data, int count );
	void	uniform( const std::string &name, const Vec3f *data, int count );
	void	uniform( const std::string &name, const Vec4f *data, int count );


  protected:
	  
	ID3DX11EffectVariable* getVariable(const std::string &name);
	ID3DX11EffectConstantBuffer* getConstBuffer(const std::string &name);
	ID3DX11EffectTechnique* getTechnique(const std::string &name);

	struct Obj {
		Obj() : mHandle( 0 ), mCurrentTech(0) {}
		~Obj();
		
		ID3DX11Effect* mHandle;
		std::map<std::string,ID3DX11EffectVariable*>	mVarableLocs;
		std::map<std::string,ID3DX11EffectConstantBuffer*>	mConstBufferLocs;
		std::map<std::string,ID3DX11EffectTechnique*>	mTechniqueLocs;
		ID3DX11EffectTechnique*							mCurrentTech;
	};
 
	std::shared_ptr<Obj>	mObj;

  public:
	//@{
	//! Emulates shared_ptr-like behavior
	typedef std::shared_ptr<Obj> HlslEffect::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &HlslEffect::mObj; }
	void reset() { mObj.reset(); }
	//@}  
};

} } // namespace cinder::gl
