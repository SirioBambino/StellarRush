#pragma once

#include "Texture.h"
#include "VertexBufferObjectIndexed.h"

// Class for generating a unit sphere
class CSphere
{
public:
	CSphere();
	~CSphere();

	void Create(string filename, int slicesIn, int stacksIn);
	void Render();
	void Release();

private:
	UINT m_vao;
	CVertexBufferObjectIndexed m_vbo;
	CTexture m_texture;
	int m_numTriangles;
};