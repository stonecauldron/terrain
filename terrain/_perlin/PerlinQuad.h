#pragma once
#include "icg_common.h"
#define GRAD_SIZE 16

class PerlinQuad {
protected:
    GLuint _vao;
    GLuint _pid;
    GLuint _vbo;
    GLuint _grad_tex;
    GLfloat _grad[GRAD_SIZE * 3];
public:
    GLfloat frequency = 0.9f;
    GLfloat H = 1.0f; 
    GLfloat lacunarity = 2.7f;
    GLuint octaves = 8;

    void init() {
        _pid = opengp::load_shaders("_perlin/perlin_vshader.glsl",
                "_perlin/perlin_fshader.glsl");
        if (!_pid) exit(EXIT_FAILURE);
        glUseProgram(_pid);
        
        ///--- Vertex one vertex Array
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
     
        ///--- Vertex coordinates
        {
            const GLfloat vpoint[] = { /*V1*/ -1.0f, -1.0f, 0.0f, 
                                       /*V2*/ +1.0f, -1.0f, 0.0f, 
                                       /*V3*/ -1.0f, +1.0f, 0.0f,
                                       /*V4*/ +1.0f, +1.0f, 0.0f };        
            ///--- Buffer
            glGenBuffers(1, &_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);
        
            ///--- Attribute
            GLuint vpoint_id = glGetAttribLocation(_pid, "vpoint");
            glEnableVertexAttribArray(vpoint_id);
            glVertexAttribPointer(vpoint_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }

        ///--- Texture coordinates
        {
            const GLfloat vtexcoord[] = {/*V1*/ 0.0f, 0.0f,
                                         /*V2*/ 1.0f, 0.0f,
                                         /*V3*/ 0.0f, 1.0f,
                                         /*V4*/ 1.0f, 1.0f};
            ///--- Buffer
            glGenBuffers(1, &_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vtexcoord), vtexcoord, GL_STATIC_DRAW);

            ///--- Attribute
            GLuint vtexcoord_id = glGetAttribLocation(_pid, "vtexcoord");
            glEnableVertexAttribArray(vtexcoord_id);
            glVertexAttribPointer(vtexcoord_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }

        {
            ///--- Gradient texture
            fill_grad_tex();
            glGenTextures(1, &_grad_tex);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_1D, _grad_tex);
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, GRAD_SIZE, 0, GL_RGB, GL_FLOAT, _grad);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

            ///--- uniforms
            GLuint grad_id = glGetUniformLocation(_pid, "grad");
            glUniform1i(grad_id, 0);
        }

        ///--- to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void draw() {
        glUseProgram(_pid);
        glBindVertexArray(_vao);
            ///--- Bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_1D, _grad_tex);

            // Setup uniforms
            GLuint frequency_id = glGetUniformLocation(_pid, "initial_freq");
            GLuint H_id = glGetUniformLocation(_pid, "H");
            GLuint lacunarity_id = glGetUniformLocation(_pid, "lacunarity");
            GLuint octaves_id = glGetUniformLocation(_pid, "octaves");
            glUniform1f(frequency_id, frequency);
            glUniform1f(H_id, H);
            glUniform1f(lacunarity_id, lacunarity);
            glUniform1i(octaves_id, octaves);

            ///--- Draw
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
        glUseProgram(0);
    }
private:
    void fill_grad_tex() {
        GLfloat grad[GRAD_SIZE * 3] = {
            0.7603266842957805,-0.6495408633394705,0, 0.8809657602279946,0.47318001786414404,0, -0.5466367864419548,0.8373698249330536,0, -0.13296468949966458,-0.9911207753580074,0, 0.201892960546683,-0.9794075926199958,0, -0.04842254529095715,0.9988269405195002,0, 0.8367694012715634,0.5475554484210976,0, 0.9727549736470312,-0.2318356341138343,0, 0.7923240671229802,0.6101004611190678,0, -0.0018588171345380177,-0.9999982723979378,0, -0.6190943565348803,0.7853166098502327,0, 0.41455599418515415,0.9100238061090262,0, -0.8799918653547627,0.4749887545084044,0, -0.9837026492057273,-0.17980294198269908,0, -0.7117149151989826,0.7024684188512,0, -0.806249556332093,-0.5915755682195666,0
        };
        for (unsigned int i = 0; i < GRAD_SIZE * 3; i++) {
            _grad[i] = grad[i];
        }
    }
};
