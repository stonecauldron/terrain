#include "icg_common.h"

namespace {

GLfloat skyboxVertices[] = {
        // Positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
}

class Skybox {
protected:
    GLuint _vao; ///< vertex array object
    GLuint _pid; ///< GLSL shader program ID
    GLuint _vbo; ///< memory buffer
    GLuint _tex; ///< Texture ID
    mat4   _M;   ///< model matrix

    vector<const GLchar*> faces;
    GLuint cubemapTexture;

public:
    void init(){
        ///--- Compile the shaders
        _pid = opengp::load_shaders("_skybox/skybox_vshader.glsl", "_skybox/skybox_fshader.glsl");
        if(!_pid) exit(EXIT_FAILURE);
        glUseProgram(_pid);

        ///--- Vertex one vertex Array
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        ///--- Vertex coordinates
        {
            ///--- Buffer
            glGenBuffers(1, &_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices[0], GL_STATIC_DRAW);

            ///--- Attribute
            GLuint vpoint_id = glGetAttribLocation(_pid, "vpoint");
            glEnableVertexAttribArray(vpoint_id);
            glVertexAttribPointer(vpoint_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }


        glGenVertexArrays(1, &_vao);
        glGenBuffers(1, &_vbo);
        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glBindVertexArray(0);

        //Assign the names of the textures and load them
        faces.push_back("_skybox/rt.tga");
        faces.push_back("_skybox/lt.tga");
        faces.push_back("_skybox/up.tga");
        faces.push_back("_skybox/dn.tga");
        faces.push_back("_skybox/bk.tga");
        faces.push_back("_skybox/ft.tga");
        cubemapTexture = loadCubemap(faces);


        ///--- to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);

        ///--- Create the model matrix (remember OpenGL is right handed)
        ///--- Accumulated transformation
        typedef Eigen::Transform<float,3,Eigen::Affine> Transform;
        Transform _M = Transform::Identity();
        //_M *= Eigen::AngleAxisf(M_PI/4.0f, Eigen::Vector3f::UnitZ());
        //_M *= Eigen::AlignedScaling3f(5, 5, 5);
        //_M *= Eigen::Translation3f(0, -1, 0);
        this->_M = _M.matrix();
    }

    //Function to load the cubemap
    GLuint loadCubemap(vector<const GLchar*> faces) {

        GLuint textureID;
        glGenTextures(1, &textureID);
        glActiveTexture(GL_TEXTURE0);

        int width, height;

        GLFWimage image;

        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        for (GLuint i = 0; i < faces.size(); i++) {
            if (glfwReadImage(faces[i], &image, GLFW_ORIGIN_UL_BIT)) {
                glTexImage2D(
                        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                        image.Format, image.Width, image.Height, 0, image.Format, GL_UNSIGNED_BYTE,
                        image.Data
                        );
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        return textureID;
    }

    void cleanup(){
        /// TODO cleanup
    }

    void draw(const mat4& V, const mat4& P){

        glDepthMask(GL_FALSE);
        glUseProgram(_pid);
        glBindVertexArray(_vao);

            mat4 skyV = V;
            skyV(0,3) = 0; skyV(1,3) = 0; skyV(2,3) = 0;
            mat4 VP = P * skyV;
            ///--- Setup MVP
            mat4 MVP = VP * _M;
            GLuint MVP_id = glGetUniformLocation(_pid, "MVP");
            glUniformMatrix4fv(MVP_id, 1, GL_FALSE, MVP.data());

            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);


        glBindVertexArray(0);
        glDepthMask(GL_TRUE);
        glUseProgram(0);
    }
};
