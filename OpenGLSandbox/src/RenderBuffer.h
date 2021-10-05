#pragma once

class RenderBuffer {
private:
	unsigned int m_RendererId;
public:
	RenderBuffer( int width, int height );
	~RenderBuffer();
	void Bind() const;
	void Unbind() const;

	inline unsigned int GetRendererId() { return m_RendererId; }
private:
	int m_Width, m_Height;
};