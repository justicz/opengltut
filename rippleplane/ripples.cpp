#define GLM_FORCE_RADIANS

#define X_STRIDE 0.3f
#define Y_STRIDE 0.3f
#define GRID_SIZE 20

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <thread>
#include <string>
#include <fstream>
#include <sstream>

class GLUint;

void print_compilation_error(unsigned int shader) {
    char buffer[512];
    glGetShaderInfoLog(shader, 512, NULL, buffer);
    printf("Compilation Error:\n\n%s\n", buffer);
}

std::string read_file_to_cstr(const char* filename)
{
    std::ifstream fs(filename);
    std::stringstream ss;
    ss << fs.rdbuf();
    std::string frag = ss.str();
    return frag;
}

unsigned int compile_fragment_shader()
{
    GLint status;
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    std::string frag = read_file_to_cstr("frag.glsl");
    const char* frag_src = frag.c_str();

    glShaderSource(fragmentShader, 1, &frag_src, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE) {
        printf("compiled fragment shader\n"); 
    } else {
        printf("fragment shader compilation failed!\n");
        print_compilation_error(fragmentShader);
        exit(1);
    }

    return fragmentShader;
}

unsigned int compile_vertex_shader()
{
    GLint status;
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    std::string vert = read_file_to_cstr("vert.glsl");
    const char* vert_src = vert.c_str();

    glShaderSource(vertexShader, 1, &vert_src, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE) {
        printf("compiled vertex shader\n"); 
    } else {
        printf("vertex shader compilation failed!\n");
        print_compilation_error(vertexShader);
        exit(1);
    }

    return vertexShader;
}

glm::vec3 get_translation(int x, int y, float time) {
    return glm::vec3(x*X_STRIDE, y*Y_STRIDE, 1.5f *sin(0.5f * time + x/7.0f + y/9.0f + 1.6f));
}

glm::vec3 get_color(int x, int y, float time) {
    return glm::vec3(sin(0.2f * time + x/20.0f + y/20.0f), 0.7f, 1.0f);
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "ripples", nullptr, nullptr);

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
        //Position    //Texcoords
        -0.1f,  0.1f, 0.0f, 0.0f,
         0.1f,  0.1f, 1.0f, 0.0f,
         0.1f, -0.1f, 1.0f, 1.0f,
        -0.1f, -0.1f, 0.0f, 1.0f
    };


    // Set up the main vertex buffer
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Compile the shaders
    GLuint fragmentShader = compile_fragment_shader();
    GLuint vertexShader = compile_vertex_shader();
 
    // Initializing the shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // select an output from the fragment shader (unnecessary here since there's only one)
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);


    // identify the position attribute in our vertex buffer
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);   
    glEnableVertexAttribArray(posAttrib);
 
    // identify the texture coordinate attribute in our vertex buffer
    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));

    glm::mat4 view = glm::lookAt(
        glm::vec3(0.6f, 0.6f, 1.6f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    GLint uniView = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
    
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1920.0f/1080.0f, 0.01f, 20.0f);
    GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

    // Set up the element buffer
    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);    

    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    GLint uniFade = glGetUniformLocation(shaderProgram, "Fade");
    GLint uniColor = glGetUniformLocation(shaderProgram, "Color");

    auto t_start = std::chrono::high_resolution_clock::now();
    int x, y;    
    
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    while(!glfwWindowShouldClose(window))
    {
        auto t_now = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
        
        glm::mat4 model;
        glm::mat4 trns;

        model = glm::rotate(model, time*glm::radians(10.0f), glm::vec3(0.1f, 0.3f, 1.0f));
        
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

        glUniform1f(uniFade, (sin(0.5f * time) + 1.0f) / 2.0f);

        glfwSwapBuffers(window);
        glfwPollEvents();
        glDepthFunc(GL_LESS);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //int size = (int)(10*sin(3.0f*time) + 10);

        for (x = -GRID_SIZE; x < GRID_SIZE; x++) {
            for (y = -GRID_SIZE; y <GRID_SIZE; y++) {
                trns = glm::translate(model, get_translation(x, y, time));
                glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(trns));
                glUniform3fv(uniColor, 1, glm::value_ptr(get_color(x, y, time)));
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }
    }

    glfwTerminate();
}
