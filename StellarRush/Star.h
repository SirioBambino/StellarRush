#pragma once

#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"

// Class for generating a 4 pointed star
class CStar
{
public:
	CStar();
	~CStar();

	void Create(string filename);
	void Render();
	void Release();

private:
	GLuint m_uiVAO;
	CVertexBufferObject m_VBO;
	CTexture m_tTexture;
};
