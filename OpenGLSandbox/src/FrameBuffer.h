#pragma once
#include "Renderer.h"
#include "RenderBuffer.h"
#include "Texture.h"

class FrameBuffer {
private:
	unsigned int m_RendererId;
public:
	FrameBuffer();
	
	~FrameBuffer();
	
	void Bind() const;
	void Unbind() const;
	
	void AddColorAttachment( Texture &texture ) const;
	void AddDepthStencilAttachment( RenderBuffer &rbo ) const;
	
	void AddDepthAttachment() const;
	void AddStencilAttachment() const;

	bool IsComplete() const;
};