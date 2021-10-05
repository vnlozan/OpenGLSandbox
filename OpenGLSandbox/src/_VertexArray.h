#pragma once

class _VertexBuffer;

class _VertexArray {
private:
	unsigned int m_RendererId;
	unsigned int m_LastAttribPointer;
public:
	_VertexArray();
	~_VertexArray();
	void Bind() const;
	void Unbind() const;
	void AddBuffer( const _VertexBuffer& vb );
};