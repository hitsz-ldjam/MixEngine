#pragma once
#ifndef MX_COMMON_ENUM_H_
#define MX_COMMON_ENUM_H_
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

    enum class RenderType {
        Opaque,
        Transparent
    };

    const char* ToString(RenderType e);


    /**
     * \brief Enumeration that represents the usage of a gpu buffer and usually makes the storage location of GPU buffer different.
     */
    enum class GPUBufferUsage {
        Static,
        Dynamic,
        Stream,
        Storage
    };

    const char* ToString(GPUBufferUsage e);


    enum class GPUQueueType {
        Graphics,
        Compute,
        Transfer,

        Count /**< Indicates the number of the enumeration */
    };

    const char* ToString(GPUQueueType e);


    enum class BlendFactor {
        Zero,
        One,

        SrcColor,
        DstColor,

        OneMinusSrcColor,
        OneMinusDstColor,

        SrcAlpha,
        DstAlpha,

        OneMinusSrcAlpha,
        OneMinusDstAlpha
    };

    const char* ToString(BlendFactor e);


    enum class BlendOp {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max
    };

    const char* ToString(BlendOp e);


    enum class ColorComponent {
        R = 0x0001,
        G = 0x0002,
        B = 0x0004,
        A = 0x0008
    };

    const char* ToString(ColorComponent e);

    MX_ALLOW_FLAGS_FOR_ENUM(ColorComponent);


    enum class CompareOp {
        Never,
        Always,
        Less,
        Greater,
        LessOrEqual,
        GreaterOrEqual,
        Equal,
        NotEqual,
    };

    const char* ToString(CompareOp e);


    enum class StencilOp {
        Keep,
        Zero,
        Replace,
        IncrementAndClamp,
        DecrementAndClamp,
        Invert,
        IncrementAndWrap,
        DecrementAndWrap
    };

    const char* ToString(StencilOp e);


    enum class PolygonMode {
        Point,
        Line,
        Triangle
    };

    const char* ToString(PolygonMode e);


    enum class CullMode {
        None = 0x0000,
        Front = 0x0001,
        Back = 0x0002,
        FrontAndBack = 0x0003
    };

    const char* ToString(CullMode e);


    enum class GPUProgramType {
        Vertex = 0,
        Fragment = 1,
        Geometry = 2,
        TessEvaluation = 3,
        TessControl = 4,
        Compute = 5
    };

    const char* ToString(GPUProgramType e);

}

#endif