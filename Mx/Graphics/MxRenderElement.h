#pragma once
#ifndef MX_RENDER_ELEMENT_H_
#define MX_RENDER_ELEMENT_H_

namespace Mix {
	class Transform;
	class Mesh;
	class Material;
	class Renderer;

	struct RenderElement {
		Renderer& renderer;
		uint32_t submesh;
		Material& material;
	};
}

#endif
