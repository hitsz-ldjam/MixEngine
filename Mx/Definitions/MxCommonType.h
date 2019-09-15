#pragma once
#ifndef MX_COMMON_TYPE_H_
#define MX_COMMON_TYPE_H_
#include "../Utils/MxFlags.h"

namespace Mix {
    enum class Space {
        World,
        Self
    };

    const char* ToString(Space e);

    enum class MeshTopology {
        Points_List = 1,
        Lines_List = 2,
        Triangles_List = 3
    };

    const char* ToString(MeshTopology e);

    enum class IndexFormat {
        UInt16 = 0x0001,
        UInt32 = 0x0002
    };

    const char* ToString(IndexFormat e);

    enum class TextureWrapMode {
        Repeat = 0,
        ClampToEdge = 1,
        Mirror = 2,
    };

    const char* ToString(TextureWrapMode e);

    enum class TextureFilterMode {
        Nearest = 0,
        Linear = 1
    };

    const char* ToString(TextureFilterMode e);

    enum class TextureMipSampleMode {
        Nearest = 0,
        Linear = 1
    };

    const char* ToString(TextureMipSampleMode e);

    enum class TextureFormat {
        Unknown = 0,
        R8G8B8A8_Unorm = 1,
        B8G8R8A8_Unorm = 2
    };

    const char* ToString(TextureFormat e);

    enum class TextureType {
        Tex_2D = 0,
        Cube = 1
    };

    const char* ToString(TextureType e);

    enum class CubeMapFace {
        PositiveX = 0,
        NegativeX = 1,
        PositiveY = 2,
        NegativeY = 3,
        PositiveZ = 4,
        NegativeZ = 5,
    };

    const char* ToString(CubeMapFace e);

    /**
     * \brief This enumeration variable provides a simple and commonly used way to set up the layout of vertex data
     *	So far, Mesh data is set through setxxx(...) function,
     *	the Mesh arranges the data internally in a specific order,
     *	and uses a set of VertexAttribute to identify the layout of the vertex data.
     *	When using a Mesh, a VertexDeclaration is automatically generated so we do not need to set up the VertexDeclaration manually.
     */
    enum class VertexAttribute {
        None = 0x000,
        Position = 0x0001,
        Normal = 0x0002,
        Tangent = 0x0004,
        UV0 = 0x0008,
        UV1 = 0x0010,
        Color = 0x0020,
    };

    const char* ToString(VertexAttribute e);

    MX_ALLOW_FLAGS_FOR_ENUM(VertexAttribute);


    /**
     * \brief This enumeration is used with VertexAttribute to identify a channel of UV.
     * UVChannel::UVX has the same vaule as VertexAttribute::UVx
     */
    enum class UVChannel {
        UV0 = 0x0008,
        UV1 = 0x0010
    };

    const char* ToString(UVChannel e);
}

#endif