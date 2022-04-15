#include <stdio.h>
#include <iostream>
#include <string.h>
#include <assert.h> 
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>

GLuint VBO;
GLuint gwl;
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800

static const char* matrix_prog = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
                                                                                    \n\
uniform mat4 gWorld;                                                                \n\
                                                                                    \n\
void main(){                                                                         \n\                                                                                   \n\
    gl_Position = gWorld * vec4(Position, 1.0);                                     \n\
}";

static const char* color_prog = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
void main(){                                                                         \n\
    FragColor = vec4(0.0, 1.0, 0.0, 1.0);                                           \n\
}";

class BuilderTransformator {
private:
    glm::mat4x4 matrix, moving, resize, rotate, perspective;
public:
    BuilderTransformator() {
        moving = {
            {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
        resize = {
            {1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f}
        };
        rotate = {
            {1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f}
        };
        perspective = {
            {1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f}
        };
    }
    
    void resizeObj(float val_x, float val_y, float val_z) {
        resize = {
           {glm::sin(val_x), 0.0f, 0.0f, 0.0f},
           {0.0f, glm::sin(val_y), 0.0f, 0.0f},
           {0.0f, 0.0f, glm::sin(val_z), 0.0f},
           {0.0f, 0.0f, 0.0f, 1.0f}
        };
    }
    
    void moveObj(float x, float y, float z){
        moving = {
            {1.0f, 0.0f, 0.0f, x},
            {0.0f, 1.0f, 0.0f, y},
            {0.0f, 0.0f, 1.0f, z},
            {0.0f, 0.0f, 0.0f, 1.0f}
        };
    }
    
    void rotateObj(float angleX, float angleY, float angleZ){
        float x = glm::radians(angleX);
        float y = glm::radians(angleY);
        float z = glm::radians(angleZ);

        glm::mat4x4 rotate_X = {
            {1.0f,0.0f,0.0f,0.0f,},
            {0.0f, glm::cos(x), -glm::sin(x), 0.0f },
            {0.0f, glm::sin(x), glm::cos(x), 0.0f},
            {0.0f,0.0f,0.0f,1.0f}
        };
        glm::mat4x4 rotate_Y= {
            {glm::cos(y), 0.0f, -glm::sin(y), 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {glm::sin(y), 0.0f, glm::cos(y), 0.0f},
            {0.0f,0.0f,0.0f,1.0f}
        };
        glm::mat4x4 rotate_Z = {
             {glm::cos(z), -glm::sin(z), 0.0f, 0.0f},
            {glm::sin(z), glm::cos(z), 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
        rotate = rotate_Z * rotate_Y * rotate_X;
    }

    void perspectiveObj(float closeness, float range, float width, float height, float fov) {
        float ar = width / height;
        float zrange = closeness - range;
        float tanhal = glm::tan(glm::radians(fov / 2.0f));

        perspective = {
            {1.0f / (ar * tanhal), 0.0f, 0.0f, 0.0f},
            {0.0f, 1.0 / tanhal, 0.0f, 0.0f},
            {0.0f, 0.0f, (-closeness - range) / zrange, 2.0f * range * closeness / zrange},
            {0.0f, 0.0f, 1.0f, 0.0f}
        };
    }

    glm::mat4x4* getMatrix() {
        //matrix = perspective * moving * rotate * resize;
        matrix = perspective * resize * rotate * moving;
        return &matrix;
    }
};

static void render_scene() {
    glClear(GL_COLOR_BUFFER_BIT);

    static float v = 0.0f;
    v += 0.001f;

    BuilderTransformator move;
   
    move.resizeObj(glm::sin(v * 0.1f), glm::sin(v * 0.1f), glm::sin(v * 0.1f));
    move.moveObj(glm::sin(v), 0.0f, 0.0f);
    move.rotateObj(glm::sin(v) * 9.0f, glm::sin(v) * 9.0f, glm::sin(v) * 9.0f);
    move.perspectiveObj(1.0f, 100.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 30.0f);

    glUniformMatrix4fv(gwl, 1, GL_TRUE, (const GLfloat*)move.getMatrix());

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_POLYGON, 0, 3);

    glDisableVertexAttribArray(0);

    glutSwapBuffers();
}

static void add_shader(GLuint share_prog, const char* text_shared, GLenum type)
{
    GLuint obj = glCreateShader(type);
    if (obj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", type);
        exit(0);
    }

    const GLchar* p[1];
    p[0] = text_shared;

    GLint len[1];
    len[0] = strlen(text_shared);

    glShaderSource(obj, 1, p, len);
    glCompileShader(obj);

    GLint conn;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &conn);

    if (!conn) {
        printf("We can not connect");
        exit(1);
    }

    glAttachShader(share_prog, obj);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    
    glutCreateWindow("Lab Dmitry");
    glutDisplayFunc(render_scene);
    glutIdleFunc(render_scene);

    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: GLEW INCORRECT'\n");
        return 1;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 data[6] = {
        {0.3f, -0.3f, 0.0f, 1.0f},
        {0.5f, 0.5f, 0.0f, 1.0f},
        {-0.1f, -0.5f, 0.0f, 1.0f},
        {-0.3f, 0.3f, 0.0f, 1.0f},
        {-0.5f, -0.5f, 0.0f, 1.0f},
        {0.1f, 0.5f, 0.0f, 1.0f}
    };

    glm::vec3 square[4] = {
        { -0.9f, -0.9f, 0.1f},
        { 0.9f, -0.9f, 0.1f},
        { 0.1f, 0.9f, 0.9f},
    };

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);
    
    GLuint shader_program = glCreateProgram();

    if (shader_program == 0) {
        printf("shader prog is not created.\n");
        return -1;
    }

    add_shader(shader_program, matrix_prog, GL_VERTEX_SHADER);
    add_shader(shader_program, color_prog, GL_FRAGMENT_SHADER);
    glLinkProgram(shader_program);
    glUseProgram(shader_program);
    gwl = glGetUniformLocation(shader_program, "gWorld");
    
    if (gwl == 0xFFFFFFFF){
        printf("Error with not ");
        return -1;
    }

    glutMainLoop();

    return 0;
}
