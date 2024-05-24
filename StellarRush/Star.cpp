#include "Star.h"

CStar::CStar()
{}

CStar::~CStar()
{
	Release();
}

void CStar::Create(string filename)
{
	// Load given texture and set sampler's paramaters
	m_tTexture.Load(filename);
	m_tTexture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_tTexture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_tTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_tTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Create and bind VAO and VBO
	glGenVertexArrays(1, &m_uiVAO);
	glBindVertexArray(m_uiVAO);
	m_VBO.Create();
	m_VBO.Bind();

	// Define vertices
	glm::vec3 v0 = glm::vec3(0, 0, 1);
	glm::vec3 v1 = glm::vec3(0, 0, -1);
	glm::vec3 v2 = glm::vec3(0, 3, 0);
	glm::vec3 v3 = glm::vec3(1, 1, 0);
	glm::vec3 v4 = glm::vec3(3, 0, 0);
	glm::vec3 v5 = glm::vec3(1, -1, 0);
	glm::vec3 v6 = glm::vec3(0, -3, 0);
	glm::vec3 v7 = glm::vec3(-1, -1, 0);
	glm::vec3 v8 = glm::vec3(-3, 0, 0);
	glm::vec3 v9 = glm::vec3(-1, 1, 0);

	// Define texture coordinates
	glm::vec2 t0 = glm::vec2(0, 0);
	glm::vec2 t1 = glm::vec2(1, 1);
	glm::vec2 t2 = glm::vec2(0.75f, 0);

	// Define normals
	glm::vec3 n0 = glm::normalize((v0 - v2) * (v0 - v3));
	glm::vec3 n1 = glm::normalize((v0 - v3) * (v4 - v3)); //
	glm::vec3 n2 = glm::normalize((v4 - v5) * (v0 - v5)); //
	glm::vec3 n3 = glm::normalize((v0 - v5) * (v0 - v6));
	glm::vec3 n4 = glm::normalize((v0 - v6) * (v0 - v7));
	glm::vec3 n5 = glm::normalize((v0 - v7) * (v8 - v7)); //
	glm::vec3 n6 = glm::normalize((v8 - v9) * (v0 - v9)); //
	glm::vec3 n7 = glm::normalize((v0 - v9) * (v0 - v2));
	glm::vec3 n8 = glm::normalize((v1 - v2) * (v1 - v9));
	glm::vec3 n9 = glm::normalize((v1 - v9) * (v8 - v9)); //
	glm::vec3 n10 = glm::normalize((v8 - v7) * (v1 - v7)); //
	glm::vec3 n11 = glm::normalize((v1 - v7) * (v1 - v6));
	glm::vec3 n12 = glm::normalize((v1 - v6) * (v1 - v5));
	glm::vec3 n13 = glm::normalize((v1 - v5) * (v4 - v5)); //
	glm::vec3 n14 = glm::normalize((v4 - v3) * (v1 - v3)); //
	glm::vec3 n15 = glm::normalize((v1 - v3) * (v1 - v2));

	// Add vertex data to the VBO
	m_VBO.AddData(&v0, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&n0, sizeof(glm::vec3));
	m_VBO.AddData(&v3, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&n0, sizeof(glm::vec3));
	m_VBO.AddData(&v2, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&n0, sizeof(glm::vec3));

	m_VBO.AddData(&v0, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&n1, sizeof(glm::vec3));
	m_VBO.AddData(&v4, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&n1, sizeof(glm::vec3));
	m_VBO.AddData(&v3, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&n1, sizeof(glm::vec3));

	m_VBO.AddData(&v0, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&n2, sizeof(glm::vec3));
	m_VBO.AddData(&v5, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&n2, sizeof(glm::vec3));
	m_VBO.AddData(&v4, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&n2, sizeof(glm::vec3));

	m_VBO.AddData(&v0, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&n3, sizeof(glm::vec3));
	m_VBO.AddData(&v6, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&n3, sizeof(glm::vec3));
	m_VBO.AddData(&v5, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&n3, sizeof(glm::vec3));


	m_VBO.AddData(&v0, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&n4, sizeof(glm::vec3));
	m_VBO.AddData(&v7, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&n4, sizeof(glm::vec3));
	m_VBO.AddData(&v6, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&n4, sizeof(glm::vec3));

	m_VBO.AddData(&v0, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&n5, sizeof(glm::vec3));
	m_VBO.AddData(&v8, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&n5, sizeof(glm::vec3));
	m_VBO.AddData(&v7, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&n5, sizeof(glm::vec3));

	m_VBO.AddData(&v0, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&n6, sizeof(glm::vec3));
	m_VBO.AddData(&v9, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&n6, sizeof(glm::vec3));
	m_VBO.AddData(&v8, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&n6, sizeof(glm::vec3));

	m_VBO.AddData(&v0, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&n7, sizeof(glm::vec3));
	m_VBO.AddData(&v2, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&n7, sizeof(glm::vec3));
	m_VBO.AddData(&v9, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&n7, sizeof(glm::vec3));




	m_VBO.AddData(&v1, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&n8, sizeof(glm::vec3));
	m_VBO.AddData(&v9, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&n8, sizeof(glm::vec3));
	m_VBO.AddData(&v2, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&n8, sizeof(glm::vec3));

	m_VBO.AddData(&v1, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&n9, sizeof(glm::vec3));
	m_VBO.AddData(&v8, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&n9, sizeof(glm::vec3));
	m_VBO.AddData(&v9, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&n9, sizeof(glm::vec3));

	m_VBO.AddData(&v1, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&n10, sizeof(glm::vec3));
	m_VBO.AddData(&v7, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&n10, sizeof(glm::vec3));
	m_VBO.AddData(&v8, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&n10, sizeof(glm::vec3));

	m_VBO.AddData(&v1, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&n11, sizeof(glm::vec3));
	m_VBO.AddData(&v6, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&n11, sizeof(glm::vec3));
	m_VBO.AddData(&v7, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&n11, sizeof(glm::vec3));


	m_VBO.AddData(&v1, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&n12, sizeof(glm::vec3));
	m_VBO.AddData(&v5, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&n12, sizeof(glm::vec3));
	m_VBO.AddData(&v6, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&n12, sizeof(glm::vec3));

	m_VBO.AddData(&v1, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&n13, sizeof(glm::vec3));
	m_VBO.AddData(&v4, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&n13, sizeof(glm::vec3));
	m_VBO.AddData(&v5, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&n13, sizeof(glm::vec3));

	m_VBO.AddData(&v1, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&n14, sizeof(glm::vec3));
	m_VBO.AddData(&v3, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&n14, sizeof(glm::vec3));
	m_VBO.AddData(&v4, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&n14, sizeof(glm::vec3));

	m_VBO.AddData(&v1, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&n15, sizeof(glm::vec3));
	m_VBO.AddData(&v2, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&n15, sizeof(glm::vec3));
	m_VBO.AddData(&v3, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&n15, sizeof(glm::vec3));


	// Upload data to GPU
	m_VBO.UploadDataToGPU(GL_STATIC_DRAW);
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}

void CStar::Render()
{
	glBindVertexArray(m_uiVAO);
	m_tTexture.Bind();

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 3, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 6, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 9, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 12, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 15, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 18, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 21, 3);

	glDrawArrays(GL_TRIANGLE_STRIP, 24, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 27, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 30, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 33, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 36, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 39, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 42, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 45, 3);
}

void CStar::Release()
{
	m_tTexture.Release();
	glDeleteVertexArrays(1, &m_uiVAO);
	m_VBO.Release();
}