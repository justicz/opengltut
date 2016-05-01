#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
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

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "ripples", nullptr, nullptr);

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
        //Position     //Color           //Texcoords
        -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };


    // Set up the main vertex buffer
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set up our textures
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // Just repeat the image if the coords are > 1.0 or < 0.0
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Linearly interpolate pixels values for sampling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // Load the texture bytes into a buffer
    int width, height;
    unsigned char* image = SOIL_load_image("fox.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    printf("Loaded texture: %ipx, %ipx\n", width, height);

    // Fill the texture buffer with the image bytes
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

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
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), 0);   
    glEnableVertexAttribArray(posAttrib);
 
    // identify the color attribute in our vertex buffer
    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(2*sizeof(float)));

    // identify the texture coordinate attribute in our vertex buffer
    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(5*sizeof(float)));

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
