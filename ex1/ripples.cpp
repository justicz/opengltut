#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <thread>
#include <string>
#include <fstream>
#include <sstream>

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "ripples", nullptr, nullptr);

    glfwMakeContextCurrent(window);

    // Set up glew
    glewExperimental = GL_TRUE;
    glewInit();

    // Set up the vertex array object to save
    // how we set up attributes for our shader
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
 
    float vertices[] = {
        -0.5f,  0.5f, 0.0f, 0.0f,
         0.5f,  0.5f, 1.0f, 0.25f,
         0.5f, -0.5f, 0.1f, 0.5f,
        -0.5f, -0.5f, 0.0f, 1.0f
    };

    // Set up the main vertex buffer
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Loading and compiling the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    std::ifstream vf("vert.glsl");
    std::stringstream ssv;
    ssv << vf.rdbuf();
    std::string vert = ssv.str();
    const char *vert_src = vert.c_str();

    glShaderSource(vertexShader, 1, &vert_src, NULL);
    glCompileShader(vertexShader);

    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE) {
       printf("compiled vertex shader\n"); 
    } else {
       printf("vertex shader compilation failed!\n");
    }

    // Loading and compiling the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    std::ifstream fs("frag.glsl");
    std::stringstream ssf;
    ssf << fs.rdbuf();
    std::string frag = ssf.str();
    const char *frag_src = frag.c_str();

    glShaderSource(fragmentShader, 1, &frag_src, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE) {
       printf("compiled fragment shader\n"); 
    } else {
       printf("fragment shader compilation failed!\n");
    }

    // Initializing the shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);   
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 1, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(3*sizeof(float)));

    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);    

    while(!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    glfwTerminate();
}
