#pragma once
#include <vector>
#include "Renderer.h"
#include "Texture.h"

class Buffer {
public:
	Buffer(): m_RendererId{ 0 }, m_DeleteOnDestruct{ false } {
		GLCall( glGenBuffers( 1, &m_RendererId ) );
	}

	virtual ~Buffer() {
		//std::cout << "BUFFER DESTRUCTOR" << std::endl;
		if( m_DeleteOnDestruct ) {
			GLCall( glDeleteBuffers( 1, &m_RendererId ) );
		}
	}
	
	void SetDeleteOnDestruct( bool deleteOnDestruct = true ) {
		m_DeleteOnDestruct = deleteOnDestruct;
	}

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	inline int GetRendererId() const { return m_RendererId; }
protected:
	bool		 m_DeleteOnDestruct;
	unsigned int m_RendererId;
};

class IndexBuffer: public Buffer {
public:
	IndexBuffer( const void* data, unsigned int size, unsigned int count ): Buffer{}, m_Count{ count } {
		ASSERT( sizeof( unsigned int ) == sizeof( GLuint ) );
		Bind();
		GLCall( glBufferData( GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW ) );
		Unbind();
	}
	
	void Bind()		const override {
		GLCall( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_RendererId ) );
	}
	void Unbind()	const override {
		GLCall( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) );
	}
	
	inline unsigned int GetCount() const { return m_Count; }
private:
	unsigned int m_Count;
};

class VertexBuffer: public Buffer {
public:
	struct LayoutElement {
		unsigned int	glType			= GL_FLOAT;	// GL_FLOAT,GL_UNSIGNED_INT
		unsigned int	count			= 0;		// element count
		unsigned int	offset			= 0;		// pointer offset
		unsigned int	normalized		= GL_FALSE;	// GL_FALSE, GL_TRUE
		bool			isInstanced		= false;
		unsigned int	instanceDivisor = 1;
	};
	
	VertexBuffer( const void* data, unsigned int size ): Buffer{}, m_Stride{ 0 } {
		Bind();
		GLCall( glBufferData( GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW ) );
		Unbind();
	}

	void Bind()		const override {
		GLCall( glBindBuffer( GL_ARRAY_BUFFER, m_RendererId ) );
	}
	void Unbind()	const override {
		GLCall( glBindBuffer( GL_ARRAY_BUFFER, 0 ) );
	}

	void AddLayoutElement( unsigned int glType, unsigned int count, unsigned int normalized = GL_FALSE, bool isInstanced = false, unsigned int divisor = 1 ) {
		unsigned int offset = 0;
		if( !m_LayoutElements.empty() ) {
			offset = m_LayoutElements.back().offset + m_LayoutElements.back().count * sizeof( m_LayoutElements.back().glType );
		}
		m_LayoutElements.push_back( { glType, count, offset, normalized, isInstanced, divisor } );
		m_Stride += count * sizeof( glType );
	}
	
	inline std::vector<LayoutElement> GetElements() const { return m_LayoutElements; }
	inline unsigned int GetStride() const { return m_Stride; }
private:
	unsigned int				m_Stride;
	std::vector<LayoutElement>	m_LayoutElements;
};

class UniformBuffer: public Buffer {
public:
	UniformBuffer( unsigned int size, unsigned int bindingPoint ): Buffer{}, m_Size { size }, m_BindingPoint{ bindingPoint } {
		Bind();
		// Allocate memory
		GLCall( glBufferData( GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW ) );
		// Define the range of the buffer that links to a uniform binding point
		GLCall( glBindBufferRange( GL_UNIFORM_BUFFER, bindingPoint, m_RendererId, 0, size ) );
		Unbind();
	}
	void Bind()		const override {
		GLCall( glBindBuffer( GL_UNIFORM_BUFFER, m_RendererId ) );
	}
	void Unbind()	const override {
		GLCall( glBindBuffer( GL_UNIFORM_BUFFER, 0 ) );
	}
	void BufferSubData( void* ptr, unsigned int size, unsigned int offset = 0 ) const {
		Bind();
		GLCall( glBufferSubData( GL_UNIFORM_BUFFER, offset, size, ptr ) );
		Unbind();
	}
private:
	unsigned int m_Size;
	unsigned int m_BindingPoint;
};

class RenderBuffer {
public:
	RenderBuffer( int width, int height ): m_Height{ height }, m_Width{ width }, m_DeleteOnDestruct{ false } {
		GLCall( glGenRenderbuffers( 1, &m_RendererId ) );
	}
	~RenderBuffer() {
		GLCall( glDeleteRenderbuffers( 1, &m_RendererId ) );
	}

	void Bind() {
		glBindRenderbuffer( GL_RENDERBUFFER, m_RendererId );
		glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height );
	}
	void Unbind() {
		if( m_DeleteOnDestruct ) {
			glBindRenderbuffer( GL_RENDERBUFFER, 0 );
		}
	}

	inline int GetRendererId() const { return m_RendererId; }
	inline int GetWidth()	 const { return m_Width; }
	inline int GetHeight()	 const { return m_Height; }
private:
	bool			m_DeleteOnDestruct;
	unsigned int	m_RendererId;
	int				m_Width;
	int				m_Height;
};

class FrameBuffer {
public:
	FrameBuffer() {
		glGenFramebuffers( 1, &m_RendererId );
	}

	void Bind() {
		glBindFramebuffer( GL_FRAMEBUFFER, m_RendererId );
	}
	void Unbind() {
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}
	
	void AddDepthAttachment( Texture& t ) {
		Bind();
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, t.GetRendererId(), 0 );
		Unbind();
	}
	void AddStencilAttachment() {}
	void AddColorAttachment( Texture& t ) {
		Bind();
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t.GetRendererId(), 0 );
		Unbind();
	}
	/* Used for MSAA (Multisampled anti-aliasing) */
	void AddMultisampledColorAttachment( Texture& t ) {
		Bind();
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, t.GetRendererId(), 0 );
		Unbind();
	}
	void AddDepthStencilAttachment( RenderBuffer& rbo ) {
		Bind();
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo.GetRendererId() );
		Unbind();
	}
	
	
	void SetReadBuffer( GLenum mode=GL_NONE ) {
		Bind();
		glReadBuffer( mode );
		Unbind();
	}
	void SetDrawBuffer( GLenum buf=GL_NONE ) {
		Bind();
		glDrawBuffer( buf );
		Unbind();
	}

	bool IsComplete() const {
		return glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE;
	}
protected:
	unsigned int m_RendererId;
};