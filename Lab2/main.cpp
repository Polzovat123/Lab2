#include <stdio.h>
#include <string.h>
#include <assert.h> 
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>

GLuint VBO;
GLuint gwl;


static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
                                                                                    \n\
uniform mat4 gWorld;                                                                \n\
                                                                                    \n\
void main(){                                                                        \n\
    gl_Position = gWorld * vec4(Position, 1.0);                                     \n\
}";

static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
void main(){                                                                         \n\
    FragColor = vec4(0.0, 1.0, 0.0, 1.0);                                           \n\
}";

static void render_scene() {
    glClear(GL_COLOR_BUFFER_BIT);

    static float v = 0.0f;
    v += 0.001f;
    glm::mat4x4 move = {
        {1.0f, 0.0f, 0.0f, glm::sin(v)},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    glm::mat4x4 rotate = {
        {glm::cos(v), glm::sin(v) * (-1), 0.0f, 0.0f},
        {glm::sin(v), glm::cos(v) * (-1), 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    glm::mat4x4 resize = {
        {glm::sin(v), 0.0f, 0.0f, 0.0f},
        {0.0f, glm::sin(v), 0.0f, 0.0f},
        {0.0f, 0.0f, glm::sin(v), 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    glm::mat4x4 all_do = move + rotate + resize;

    glUniformMatrix4fv(gwl, 1, GL_TRUE, &all_do[0][0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

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
    
    glutInitWindowSize(1000, 800);
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

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    
    GLuint shader_program = glCreateProgram();

    if (shader_program == 0) {
        printf("shader prog is not created.\n");
        return -1;
    }

    add_shader(shader_program, pVS, GL_VERTEX_SHADER);
    add_shader(shader_program, pFS, GL_FRAGMENT_SHADER);
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
