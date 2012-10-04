//--------------------------------------------------------------------------------------
// File: VertexTypes.h
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
//--------------------------------------------------------------------------------------

#pragma once

#include "cinder/Vector.h"
#include "cinder/Color.h"
#include <d3d11.h>

namespace cinder { namespace dx11 {
// Vertex struct holding position and color information.
struct VertexPC
{
    VertexPC()
    { }

    VertexPC(const Vec3f& position, const ColorA& color)
        : position(position),
        color(color)
    { }

    Vec3f position;
    ColorA color;

    static const int InputElementCount = 2;
    static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};


// Vertex struct holding position and texture mapping information.
struct VertexPT
{
    VertexPT()
    { }

    VertexPT(const Vec3f& position, const Vec2f& texCoord)
        : position(position),
        texCoord(texCoord)
    { }


    Vec3f position;
    Vec2f texCoord;

    static const int InputElementCount = 2;
    static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};


// Vertex struct holding position and normal vector.
struct VertexPN
{
    VertexPN()
    { }

    VertexPN(const Vec3f& position, const Vec3f& normal)
        : position(position),
        normal(normal)
    { }

    Vec3f position;
    Vec3f normal;

    static const int InputElementCount = 2;
    static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};


// Vertex struct holding position, color, and texture mapping information.
struct VertexPCT
{
    VertexPCT()
    { }

    VertexPCT(const Vec3f& position, const ColorA& color, const Vec2f& texCoord)
        : position(position),
        color(color),
        texCoord(texCoord)
    { }
    Vec3f position;
    ColorA color;
    Vec2f texCoord;

    static const int InputElementCount = 3;
    static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};


// Vertex struct holding position, normal vector, and color information.
struct VertexPNC
{
    VertexPNC()
    { }

    VertexPNC(const Vec3f& position, const Vec3f& normal, const ColorA& color)
        : position(position),
        normal(normal),
        color(color)
    { }

    Vec3f position;
    Vec3f normal;
    ColorA color;

    static const int InputElementCount = 3;
    static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};


// Vertex struct holding position, normal vector, and texture mapping information.
struct VertexPNT
{
    VertexPNT()
    { }

    VertexPNT(const Vec3f& position, const Vec3f& normal, const Vec2f& texCoord)
        : position(position),
        normal(normal),
        texCoord(texCoord)
    { }

    Vec3f position;
    Vec3f normal;
    Vec2f texCoord;

    static const int InputElementCount = 3;
    static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};


// Vertex struct holding position, normal vector, color, and texture mapping information.
struct VertexPNCT
{
    VertexPNCT()
    { }

    VertexPNCT(const Vec3f& position, const Vec3f& normal, const ColorA& color, const Vec2f& texCoord)
        : position(position),
        normal(normal),
        color(color),
        texCoord(texCoord)
    { } 

    Vec3f position;
    Vec3f normal;
    ColorA color;
    Vec2f texCoord;

    static const int InputElementCount = 4;
    static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

} } // namespace cinder::dx11