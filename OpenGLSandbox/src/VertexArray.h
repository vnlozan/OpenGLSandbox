#pragma once

class VertexBuffer;

class VertexArray {
private:
	unsigned int m_RendererId;
	unsigned int m_LastAttribPointer;
public:
	VertexArray();
	~VertexArray();
	void Bind() const;
	void Unbind() const;
	void AddBuffer( const VertexBuffer& vb );
};