#pragma once
#include "icg_common.h"

class Grid{
protected:
    GLuint _vao;          ///< vertex array object
    GLuint _vbo_position; ///< memory buffer for positions
    GLuint _vbo_index;    ///< memory buffer for indice
    GLuint _pid;          ///< GLSL shader program ID
    GLuint _tex;          ///< Height map Texture
    GLuint _grass;        ///< Grass texture
    GLuint _rock;         ///< Rock texture
    GLuint _sand;         ///< Sand texture
    GLuint _sediment;     ///< Sediment texture;
    GLuint _snow;         ///< Snow texture;
    GLuint _mirror_tex;          ///< Height map Texture
    GLuint _num_indices;  ///< number of vertices to render
    mat4 _M;              ///< model matrix
    
public:
    void init(int grid_dim, GLuint texture, GLuint mirror_texture, const char* v_shader,
                const char* f_shader) {

        // Compile the shaders
        _pid = opengp::load_shaders(v_shader, f_shader);
        if(!_pid) exit(EXIT_FAILURE);       
        glUseProgram(_pid);
        
        // Vertex one vertex Array
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
     
        // Vertex coordinates and indices
        {
            std::vector<GLfloat> vertices;
            std::vector<GLuint> indices;

            for (int y = 0; y < grid_dim; y++) {
                for (int x = 0; x < grid_dim; x++) {
                    vertices.push_back(-1.0 + 2.0f * x / (grid_dim - 1.0));
                    vertices.push_back(1.0 - 2.0f * y / (grid_dim - 1.0));
                }
            }
            int primitive_restart_idx = 0xffffffff;
            glPrimitiveRestartIndex(primitive_restart_idx);
            glEnable(GL_PRIMITIVE_RESTART);

            for (int y = 0; y < grid_dim - 1; ++y) {
                for (int x = 0; x < grid_dim; ++x) {
                    indices.push_back((y + 1) * grid_dim + x);
                    indices.push_back(y * grid_dim + x);
                }
                indices.push_back(primitive_restart_idx);
            }  
            _num_indices = indices.size();

            // position buffer
            glGenBuffers(1, &_vbo_position);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo_position);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

            // vertex indices
            glGenBuffers(1, &_vbo_index);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo_index);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

            // position shader attribute
            GLuint loc_position = glGetAttribLocation(_pid, "position");
            glEnableVertexAttribArray(loc_position);
            glVertexAttribPointer(loc_position, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }

        ///--- Create the model matrix
        typedef Eigen::Transform<float,3,Eigen::Affine> Transform;
        Transform _M = Transform::Identity();
        //_M *= Eigen::AlignedScaling3f(10, 10, 10);
        this->_M = _M.matrix();
        
        ///--- Load/Assign texture
        this->_tex = texture;
        glBindTexture(GL_TEXTURE_2D, _tex);
        glBindTexture(GL_TEXTURE_2D, 0);

        ///--- Load/Assign texture
        this->_mirror_tex = mirror_texture;
        glBindTexture(GL_TEXTURE_2D, _mirror_tex);
        glBindTexture(GL_TEXTURE_2D, 0);

        ///--- Load Grass texture
        glGenTextures(1, &_grass);
        glBindTexture(GL_TEXTURE_2D, _grass);
        glfwLoadTexture2D("_grid/textures/grass.tga", 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);

        ///--- Load rock texture
        glGenTextures(1, &_rock);
        glBindTexture(GL_TEXTURE_2D, _rock);
        glfwLoadTexture2D("_grid/textures/rock2.tga", 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);

        ///--- Load sand texture
        glGenTextures(1, &_sand);
        glBindTexture(GL_TEXTURE_2D, _sand);
        glfwLoadTexture2D("_grid/textures/sand.tga", 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);

        ///--- Load sediment texture
        glGenTextures(1, &_sediment);
        glBindTexture(GL_TEXTURE_2D, _sediment);
        glfwLoadTexture2D("_grid/textures/sand2.tga", 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);

        ///--- Load snow texture
        glGenTextures(1, &_snow);
        glBindTexture(GL_TEXTURE_2D, _snow);
        glfwLoadTexture2D("_grid/textures/snow2.tga", 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);

        ///--- to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }
           
    void cleanup(){
        glDeleteBuffers(1, &_vbo_position);
        glDeleteBuffers(1, &_vbo_index);
        glDeleteVertexArrays(1, &_vao);
        glDeleteProgram(_pid);
        glDeleteTextures(1, &_tex);
        glDeleteTextures(1, &_grass);
    }
    
    void draw(const mat4& VP){
        glUseProgram(_pid);
        glBindVertexArray(_vao);

        // Bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _tex);
        GLuint tex_id = glGetUniformLocation(_pid, "tex");
        glUniform1i(tex_id, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _grass);
        GLuint grass_id = glGetUniformLocation(_pid, "grass");
        glUniform1i(grass_id, 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, _rock);
        GLuint rock_id = glGetUniformLocation(_pid, "rock");
        glUniform1i(rock_id, 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, _sediment);
        GLuint sediment_id = glGetUniformLocation(_pid, "sediment");
        glUniform1i(sediment_id, 3);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, _sand);
        GLuint sand_id = glGetUniformLocation(_pid, "sand");
        glUniform1i(sand_id, 4);

        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, _snow);
        GLuint snow_id = glGetUniformLocation(_pid, "snow");
        glUniform1i(snow_id, 5);
        
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, _mirror_tex);
        GLuint mirror_tex_id = glGetUniformLocation(_pid, "mirror_tex");
        glUniform1i(mirror_tex_id, 6);
        // Setup MVP
        mat4 MVP = VP * _M;
        GLuint MVP_id = glGetUniformLocation(_pid, "mvp");
        glUniformMatrix4fv(MVP_id, 1, GL_FALSE, MVP.data());

        glDrawElements(GL_TRIANGLE_STRIP, _num_indices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);        
        glUseProgram(0);
    }
};

