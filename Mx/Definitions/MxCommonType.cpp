#include "MxCommonType.h"

const char* Mix::ToString(Space e) {
    switch (e) {
    case Space::World: return "World";
    case Space::Self: return "Self";
    default: return "Unknown";
    }
}

const char* Mix::ToString(MeshTopology e) {
    switch (e) {
    case MeshTopology::Points_List: return "Points_List";
    case MeshTopology::Lines_List: return "Lines_List";
    case MeshTopology::Triangles_List: return "Triangles_List";
    default: return "Unknown";
    }
}

const char* Mix::ToString(IndexFormat e) {
    switch (e) {
    case IndexFormat::UInt16: return "UInt16";
    case IndexFormat::UInt32: return "UInt32";
    default: return "Unknown";
    }
}

const char* Mix::ToString(TextureWrapMode e) {
    switch (e) {
    case TextureWrapMode::Repeat: return "Repeat";
    case TextureWrapMode::ClampToEdge: return "ClampToEdge";
    case TextureWrapMode::Mirror: return "Mirror";
    default: return "Unknown";
    }
}

const char* Mix::ToString(TextureFilterMode e) {
    switch (e) {
    case TextureFilterMode::Nearest: return "Nearest";
    case TextureFilterMode::Linear: return "Linear";
    default: return "Unknown";
    }
}

const char* Mix::ToString(TextureMipSampleMode e) {
    switch (e) {
    case TextureMipSampleMode::Nearest: return "Nearest";
    case TextureMipSampleMode::Linear: return "Linear";
    default: return "Unknown";
    }
}

const char* Mix::ToString(TextureFormat e) {
    switch (e) {
    case TextureFormat::Unknown: return "Unknown";
    case TextureFormat::R8G8B8A8_Unorm: return "R8G8B8A8_Unorm";
    case TextureFormat::B8G8R8A8_Unorm: return "B8G8R8A8_Unorm";
    default: return "Unknown";
    }
}

const char* Mix::ToString(TextureType e) {
    switch (e) {
    case TextureType::Tex_2D: return "Tex_2D";
    case TextureType::Cube: return "Cube";
    default: return "Unknown";
    }
}

const char* Mix::ToString(CubeMapFace e) {
    switch (e) {
    case CubeMapFace::PositiveX: return "PositiveX";
    case CubeMapFace::NegativeX: return "NegativeX";
    case CubeMapFace::PositiveY: return "PositiveY";
    case CubeMapFace::NegativeY: return "NegativeY";
    case CubeMapFace::PositiveZ: return "PositiveZ";
    case CubeMapFace::NegativeZ: return "NegativeZ";
    default: return "Unknown";
    }
}

const char* Mix::ToString(VertexAttribute e) {
    switch (e) {
    case VertexAttribute::None:return "None";
    case VertexAttribute::Position: return "Position";
    case VertexAttribute::Normal: return "Normal";
    case VertexAttribute::Tangent: return "Tangent";
    case VertexAttribute::UV0: return "Uv0";
    case VertexAttribute::UV1: return "Uv1";
    case VertexAttribute::Color: return "Color";
    default: return "Unknown";
    }
}

const char* Mix::ToString(UVChannel e) {
    switch (e) {
    case UVChannel::UV0: return "UV0";
    case UVChannel::UV1: return "UV1";
    default: return "Unknown";
    }
}

const char* Mix::ToString(RenderType e) {
    switch (e) {
    case RenderType::Opaque: return "Opaque";
    case RenderType::Transparent: return "Transparent";
    default: return "Unknown";
    }
}
