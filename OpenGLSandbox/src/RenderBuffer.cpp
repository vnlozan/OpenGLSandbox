#include "RenderBuffer.h"
#include "Renderer.h"

RenderBuffer::RenderBuffer( int width, int height ) : m_Height{ height }, m_Width{ width } {
	glGenRenderbuffers( 1, &m_RendererId );
}
RenderBuffer::~RenderBuffer() {}
void RenderBuffer::Bind() const {
	glBindRenderbuffer( GL_RENDERBUFFER, m_RendererId );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height );
}
void RenderBuffer::Unbind() const {
	glBindRenderbuffer( GL_RENDERBUFFER, 0 );
}