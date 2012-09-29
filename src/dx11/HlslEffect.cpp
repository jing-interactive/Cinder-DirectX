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

#include "dx11/dx11.h"
#include "dx11/HlslEffect.h"

using namespace std;

namespace cinder { namespace dx11 {

HlslEffect::Obj::~Obj()
{
	SAFE_RELEASE(mHandle);
}

//////////////////////////////////////////////////////////////////////////
// HlslProg
    HlslEffect::HlslEffect( DataSourceRef effect)
	: mObj( shared_ptr<Obj>( new Obj ) )
{
	HRESULT hr = S_OK;
	V(dx11::createEffect(effect, &mObj->mHandle));
}
 
void HlslEffect::uniform( const std::string &name, int data )
{
	ID3DX11EffectVariable* var = getVariable( name );
	var->AsScalar()->SetInt(data);
	//glUniform1i( loc, data );
}

void HlslEffect::uniform( const std::string &name, const Vec2i &data )
{
	ID3DX11EffectVariable* var = getVariable( name );
	//glUniform2i( loc, data.x, data.y );
}

void HlslEffect::uniform( const std::string &name, const int *data, int count )
{
	ID3DX11EffectVariable* var = getVariable( name );
	//glUniform1iv( loc, count, data );
}

void HlslEffect::uniform( const std::string &name, const Vec2i *data, int count )
{
	ID3DX11EffectVariable* var = getVariable( name );
	//glUniform2iv( loc, count, &data[0].x );
}

void HlslEffect::uniform( const std::string &name, float data )
{
	ID3DX11EffectVariable* var = getVariable( name );
	//glUniform1f( loc, data );
}

void HlslEffect::uniform( const std::string &name, const Vec2f &data )
{
	ID3DX11EffectVariable* var = getVariable( name );
	//glUniform2f( loc, data.x, data.y );
}

void HlslEffect::uniform( const std::string &name, const Vec3f &data )
{
	ID3DX11EffectVariable* var = getVariable( name );
	//glUniform3f( loc, data.x, data.y, data.z );
}

void HlslEffect::uniform( const std::string &name, const Vec4f &data )
{
	ID3DX11EffectVariable* var = getVariable( name );
	//glUniform4f( loc, data.x, data.y, data.z, data.w );
}

void HlslEffect::uniform( const std::string &name, const Color &data )
{
	ID3DX11EffectVariable* var = getVariable( name );
	//glUniform3f( loc, data.r, data.g, data.b );
}

void HlslEffect::uniform( const std::string &name, const ColorA &data )
{
	ID3DX11EffectVariable* var = getVariable( name );
	//glUniform4f( loc, data.r, data.g, data.b, data.a );
}

void HlslEffect::uniform( const std::string &name, const float *data, int count )
{
	ID3DX11EffectVariable* var = getVariable( name );
	//glUniform1fv( loc, count, data );
}

void HlslEffect::uniform( const std::string &name, const Vec2f *data, int count )
{
	ID3DX11EffectVariable* var = getVariable( name );
	//glUniform2fv( loc, count, &data[0].x );
}

void HlslEffect::uniform( const std::string &name, const Vec3f *data, int count )
{
	ID3DX11EffectVariable* var = getVariable( name );
	//glUniform3fv( loc, count, &data[0].x );
}

void HlslEffect::uniform( const std::string &name, const Vec4f *data, int count )
{
	ID3DX11EffectVariable* var = getVariable( name );
	//glUniform4fv( loc, count, &data[0].x );
}

void HlslEffect::uniform( const std::string &name, const Matrix33f &data, bool transpose )
{
	ID3DX11EffectVariable* var = getVariable( name );
	//glUniformMatrix3fv( loc, 1, ( transpose ) ? GL_TRUE : GL_FALSE, data.m );
}

void HlslEffect::uniform( const std::string &name, const Matrix44f &data, bool transpose )
{
	ID3DX11EffectVariable* var = getVariable( name );
	if (var){
		ID3DX11EffectMatrixVariable* varM = var->AsMatrix();
		if (transpose)
			varM->SetMatrixTranspose(data.m);
		else
			varM->SetMatrix(data.m);
	}
	//glUniformMatrix4fv( loc, 1, ( transpose ) ? GL_TRUE : GL_FALSE, data.m );
}

ID3DX11EffectVariable* HlslEffect::getVariable( const std::string &name )
{
	map<string,ID3DX11EffectVariable*>::const_iterator it = mObj->mVarableLocs.find( name );
	if( it == mObj->mVarableLocs.end() ) {
		ID3DX11EffectVariable* var = mObj->mHandle->GetVariableByName(name.c_str());
		mObj->mVarableLocs[name] = var;
		return var;
	}
	else
		return it->second;
}

ID3DX11EffectTechnique* HlslEffect::getTechnique( const std::string &name )
{
	map<string,ID3DX11EffectTechnique*>::const_iterator it = mObj->mTechniqueLocs.find( name );
	if( it == mObj->mTechniqueLocs.end() ) {
		ID3DX11EffectTechnique* tech = mObj->mHandle->GetTechniqueByName(name.c_str());
		mObj->mTechniqueLocs[name] = tech;
		return tech;
	}
	else
		return it->second;
}

HRESULT HlslEffect::createInputLayout(const std::string &techniqueName, const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs, 
									  UINT NumElements,ID3D11InputLayout **ppInputLayout, int pass)
{
	HRESULT hr = S_OK;
	D3DX11_PASS_DESC passDesc;

	ID3DX11EffectTechnique* tech = getTechnique(techniqueName);
	if (tech == NULL)
		return E_FAIL;
	tech->GetPassByIndex(0)->GetDesc(&passDesc);

	V_RETURN(dx11::getDevice()->CreateInputLayout(pInputElementDescs, NumElements, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, ppInputLayout));
}

void HlslEffect::draw( const std::string &techniqueName, UINT VertexCount, UINT StartVertexLocation )
{
	ID3DX11EffectTechnique* tech = getTechnique(techniqueName);
	if (tech)
	{
		dx11::drawWithTechnique(tech, VertexCount, StartVertexLocation);
	}
}

void HlslEffect::drawIndexed( const std::string &techniqueName, UINT VertexCount, UINT StartVertexLocation, INT BaseVertexLocation )
{
	ID3DX11EffectTechnique* tech = getTechnique(techniqueName);
	if (tech)
	{
		dx11::drawIndexedWithTechnique(tech, VertexCount, StartVertexLocation, BaseVertexLocation);
	}
}



} } // namespace cinder::dx11
