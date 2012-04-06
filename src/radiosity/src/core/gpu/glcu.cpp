#include <iostream>

#include <GL/glew.h>
#include <GL/glut.h>

#include <cuda_runtime_api.h>
#include <cuda_gl_interop.h>

#include "glcu.hpp"

int width = 512;
int height = 512;

GLuint frame_buffer_id = 0;
GLuint depth_buffer_id = 0;
GLuint color_buffer_id = 0;
cudaGraphicsResource_t cuda_resource;

float rotation[3] = { 0.0f, 0.0f, 0.0f };  // Rotation parameter for scene object.

#if 0
int main(int argc, char **argv)
{
    if (!init_gl(argc, argv))
    {
        std::cerr << "ERROR: Failed to initialize OpenGL" << std::endl;
    }
    init_cuda();
    //glutMainLoop();
    reshape(width, height);
    display();
    //cg::glcu(color_buffer_id, width, height);
    return 0;
};
#endif

namespace glcu = cg::glcu;

bool glcu::init_gl(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("glcu");
    glutDisplayFunc(&display);
    glutReshapeFunc(&reshape);
    //glutIdleFunc(&render_scene);

    // Init GLEW
    auto err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << "Error: " << glewGetErrorString(err);
        return false;
    }
    if (!glewIsSupported(
        "GL_VERSION_3_1 " 
        "GL_ARB_pixel_buffer_object "
        "GL_ARB_framebuffer_object "
        "GL_ARB_copy_buffer " 
        ))
    {
        std::cerr << "Low GL version.\n";
        return false;
    }

    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    glDisable( GL_DEPTH_TEST );

    // Setup the viewport
    glViewport(0, 0, width, height);

    // Setup the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective( 60.0, (GLdouble)width /(GLdouble)height, 0.1, 1.0 );
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    // Enable one light.
    glEnable( GL_LIGHT0 );
    const GLfloat fRed[] = { 1.0f, 0.1f, 0.1f, 1.0f };
    const GLfloat fWhite[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, fRed );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, fWhite );
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 60.0f );
    return true;
}

void glcu::render_scene()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 60.0, (GLdouble)width / (GLdouble)height, 0.1, 10.0 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatef( 0.0f, 0.0f, -3.0f );

    glRotatef( rotation[0], 1.0f, 0.0f, 0.0f );
    glRotatef( rotation[1], 0.0f, 1.0f, 0.0f );
    glRotatef( rotation[2], 0.0f, 0.0f, 1.0f );

    glViewport( 0, 0, width, height );

    glEnable( GL_LIGHTING );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );

    glutSolidTeapot( 1.0 );
}

void glcu::reshape(int w, int h)
{
    h = std::max(h, 1);

    width = w;
    height = h;

    render_to_memory(color_buffer_id, depth_buffer_id, frame_buffer_id, cuda_resource, width, height);

    glutPostRedisplay();
}

void glcu::diaplay_image(GLuint texture, int x, int y, int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    glMatrixMode( GL_MODELVIEW);
    glLoadIdentity();

    glPushAttrib( GL_VIEWPORT_BIT );
    glViewport(x, y, width, height );

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, 0.5);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, 0.5);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 0.5);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, 0.5);
    glEnd();

    glPopAttrib();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

void glcu::delete_texture(GLuint &texture)
{
    if (texture)
    {
        glDeleteTextures(1, &texture);
        texture = 0;
    }
}

void glcu::create_texture(GLuint &texture, int width, int height)
{
    // Make sure we don't already have a texture defined here
    delete_texture( texture );

    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );

    // set basic parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Create texture data (4-component unsigned byte)
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

    // Unbind the texture
    glBindTexture( GL_TEXTURE_2D, 0 );
}

void glcu::delete_depth_buffer(GLuint &depth_buffer)
{
    if (depth_buffer)
    {
        glDeleteRenderbuffers(1, &depth_buffer);
        depth_buffer = 0;
    }
}

void glcu::create_depth_buffer(GLuint &depth_buffer, int width, int height)
{
    // delete the existing depth buffer if there is one
    delete_depth_buffer(depth_buffer);
    glGenRenderbuffers(1, &depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    // unbind the depth buffer
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
}

void glcu::delete_frame_buffer(GLuint &frame_buffer)
{
    if (frame_buffer)
    {
        glDeleteFramebuffers(1, &frame_buffer);
        frame_buffer = 0;
    }
}

void glcu::create_frame_buffer(
    GLuint &frame_buffer,
    GLuint color_attachment0,
    GLuint depth_attachment
    )
{
    // delete the existing frame buffer if it exists
    delete_frame_buffer(frame_buffer);
    glGenFramebuffers(1, &frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment0, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_attachment);
    // check to see if the frame buffer is valid
    if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
    {
        std::cerr << "ERROR: Incomplete framebuffer status." << std::endl;
    }
    // unbind the frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void glcu::create_cuda_resource(
    cudaGraphicsResource_t &resource,
    GLuint texture,
    cudaGraphicsMapFlags flags
    )
{
    cudaGraphicsGLRegisterImage(&resource, texture, GL_TEXTURE_2D, flags);
}

void glcu::render_to_memory(
    GLuint &color_buffer_id,
    GLuint &depth_buffer_id,
    GLuint &frame_buffer_id,
    cudaGraphicsResource_t &cuda_resource,
    int width, int height
    )
{
    // Create a surface texture to render the scene to.
    create_texture(color_buffer_id, width, height);
    // Create a depth buffer for the frame buffer object.
    create_depth_buffer(depth_buffer_id, width, height);
    // Attach the color and depth textures to the framebuffer.
    create_frame_buffer(frame_buffer_id, color_buffer_id, depth_buffer_id);
    // Map the color attachment to a CUDA graphics resource so we can read it in a CUDA a kernel.
    create_cuda_resource(cuda_resource, color_buffer_id, cudaGraphicsMapFlagsReadOnly);
}

void glcu::display()
{
    // Bind the framebuffer that we want to use as the render target.
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);
    render_scene();
    // Unbind the framebuffer so we render to the back buffer again.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    test(cuda_resource, width, height);

    glutSwapBuffers();
    glutPostRedisplay();
}

void glcu::init_cuda()
{
    cudaDeviceProp prop = {0};
    int dev;
    prop.major = 1;
    prop.minor = 0;
    HANDLE_ERROR(cudaChooseDevice(&dev, &prop));
    HANDLE_ERROR(cudaGLSetGLDevice(dev));
}

void glcu::delete_cuda_resource(cudaGraphicsResource_t &resource)
{
    if (resource)
    {
        HANDLE_ERROR(cudaGraphicsUnregisterResource(resource));
        resource = 0;
    }
}