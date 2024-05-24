#pragma once

#include "Texture.h"
#include "VertexBufferObject.h"
#include "FrameBufferObject.h"

// Class for generating a xz plane of a given size
class CQuad
{
public:
	CQuad();
	~CQuad();

	void Create(string filename);
	void Render(bool bindTexture = true);
	void Release();
private:
	UINT m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
};