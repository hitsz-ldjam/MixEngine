#include "MxCommonEnum.h"

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

const char* Mix::ToString(GPUBufferUsage e) {
    switch (e) {
    case GPUBufferUsage::Static: return "Static";
    case GPUBufferUsage::Dynamic: return "Dynamic";
    case GPUBufferUsage::Stream: return "Stream";
    case GPUBufferUsage::Storage: return "Transfer";
    default: return "Unknown";
    }
}

const char* Mix::ToString(GPUQueueType e) {
    switch (e) {
    case GPUQueueType::Graphics: return "Graphics";
    case GPUQueueType::Compute: return "Compute";
    case GPUQueueType::Transfer: return "Transfer";
    default: return "Unknown";
    }
}

const char* Mix::ToString(BlendFactor e) {
    switch (e) {
    case BlendFactor::Zero: return "Zero";
    case BlendFactor::One: return "One";
    case BlendFactor::SrcColor: return "SrcColor";
    case BlendFactor::DstColor: return "DstColor";
    case BlendFactor::OneMinusSrcColor: return "OneMinusSrcColor";
    case BlendFactor::OneMinusDstColor: return "OneMinusDstColor";
    case BlendFactor::SrcAlpha: return "SrcAlpha";
    case BlendFactor::DstAlpha: return "DstAlpha";
    case BlendFactor::OneMinusSrcAlpha: return "OneMinusSrcAlpha";
    case BlendFactor::OneMinusDstAlpha: return "OneMinusDstAlpha";
    default: return "Unknown";
    }
}

const char* Mix::ToString(BlendOp e) {
    switch (e) {
    case BlendOp::Add: return "Add";
    case BlendOp::Subtract: return "Subtract";
    case BlendOp::ReverseSubtract: return "ReverseSubtract";
    case BlendOp::Min: return "Min";
    case BlendOp::Max: return "Max";
    default: return "Unknown";
    }
}

const char* Mix::ToString(ColorComponent e) {
    switch (e) {
    case ColorComponent::R: return "R";
    case ColorComponent::G: return "G";
    case ColorComponent::B: return "B";
    case ColorComponent::A: return "A";
    default: return "Unknown";
    }
}

const char* Mix::ToString(CompareOp e) {
    switch (e) {
    case CompareOp::Never: return "Never";
    case CompareOp::Always: return "Always";
    case CompareOp::Less: return "Less";
    case CompareOp::Greater: return "Greater";
    case CompareOp::LessOrEqual: return "LessOrEqual";
    case CompareOp::GreaterOrEqual: return "GreaterOrEqual";
    case CompareOp::Equal: return "Equal";
    case CompareOp::NotEqual: return "NotEqual";
    default: return "Unknown";
    }
}

const char* Mix::ToString(StencilOp e) {
    switch (e) {
    case StencilOp::Keep: return "Keep";
    case StencilOp::Zero: return "Zero";
    case StencilOp::Replace: return "Replace";
    case StencilOp::IncrementAndClamp: return "IncrementAndClamp";
    case StencilOp::DecrementAndClamp: return "DecrementAndClamp";
    case StencilOp::Invert: return "Invert";
    case StencilOp::IncrementAndWrap: return "IncrementAndWrap";
    case StencilOp::DecrementAndWrap: return "DecrementAndWrap";
    default: return "Unknown";
    }
}

const char* Mix::ToString(PolygonMode e) {
    switch (e) {
    case PolygonMode::Point: return "Point";
    case PolygonMode::Line: return "Line";
    case PolygonMode::Triangle: return "Triangle";
    default: return "Unknown";
    }
}

const char* Mix::ToString(CullMode e) {
    switch (e) {
    case CullMode::None: return "None";
    case CullMode::Front: return "Front";
    case CullMode::Back: return "Back";
    case CullMode::FrontAndBack: return "FrontAndBack";
    default: return "Unknown";
    }
}

const char* Mix::ToString(GPUProgramType e) {
    switch (e) {
    case GPUProgramType::Vertex: return "Vertex";
    case GPUProgramType::Fragment: return "Fragment";
    case GPUProgramType::Geometry: return "Geometry";
    case GPUProgramType::TessEvaluation: return "TessEvalution";
    case GPUProgramType::TessControl: return "TessControl";
    case GPUProgramType::Compute: return "Compute";
    default: return "Unknown";
    }
}
