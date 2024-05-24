#pragma once

#include "Texture.h"
#include "VertexBufferObject.h"
#include "FrameBufferObject.h"

// Class for generating a xz plane of a given size
class CPlane
{
public:
	CPlane();
	~CPlane();
	void Create(string filename, float width, float height, float textureRepeat);
	void Render(bool bindTexture = true);
	void Release();
private:
	UINT m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	float m_width;
	float m_height;
};