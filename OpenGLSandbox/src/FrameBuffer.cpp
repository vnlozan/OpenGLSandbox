#include "FrameBuffer.h"

FrameBuffer::FrameBuffer() {
	glGenFramebuffers( 1, &m_RendererId );
}
FrameBuffer::~FrameBuffer() {
}

void FrameBuffer::Bind() const {
	glBindFramebuffer( GL_FRAMEBUFFER, m_RendererId );
}
void FrameBuffer::Unbind() const {
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}
void FrameBuffer::AddColorAttachment( Texture& texture ) const {
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.GetRendererId(), 0 );
}
void FrameBuffer::AddDepthStencilAttachment( RenderBuffer& rbo ) const {
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo.GetRendererId() );
}
bool FrameBuffer::IsComplete() const {
	return glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE;
}