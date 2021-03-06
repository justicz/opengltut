#define GLM_FORCE_RADIANS

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
        //Position          //Color           //Texcoords
                
        // Bottom face
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

         0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,              
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        // Top face        
        -0.5f,  0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

         0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,              
        -0.5f,  0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        // Front face
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

         0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 
        
        // Back face
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, 0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

         0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 
        
        // Right face
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f,  0.5f,   0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        
        // Left face
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,   0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        
        // Floor
        -1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
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

    // Set up our textures
    GLuint textures[2];
    glGenTextures(2, textures);

    int width, height;
    unsigned char* image;

    ////////////////////////////////////////////
    ///////////////// FOX //////////////////////
    ////////////////////////////////////////////
    
    // Load the fox texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    image = SOIL_load_image("fox.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    printf("Loaded texture: %ipx, %ipx\n", width, height);

    // Just repeat the image if the coords are > 1.0 or < 0.0
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Linearly interpolate pixels values for sampling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Fill the texture buffer with the image bytes
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);
    glUniform1i(glGetUniformLocation(shaderProgram, "texFox"), 0);

    ////////////////////////////////////////////
    ///////////////// CAT //////////////////////
    ////////////////////////////////////////////    

    // Load the fox texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    image = SOIL_load_image("husky.png", &width, &height, 0, SOIL_LOAD_RGB);
    printf("Loaded texture: %ipx, %ipx\n", width, height);

    // Just repeat the image if the coords are > 1.0 or < 0.0
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Linearly interpolate pixels values for sampling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Fill the texture buffer with the image bytes
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);
    glUniform1i(glGetUniformLocation(shaderProgram, "texCat"), 1);
    
    //////////////////////////////////////////////////////////

    // identify the position attribute in our vertex buffer
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);   
    glEnableVertexAttribArray(posAttrib);
 
    // identify the color attribute in our vertex buffer
    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));

    // identify the texture coordinate attribute in our vertex buffer
    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));

    glm::mat4 view = glm::lookAt(
        glm::vec3(3.0f, 3.0f, 1.4f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    GLint uniView = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
    
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1.0f, 1.0f, 10.0f);
    GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    GLint uniFade = glGetUniformLocation(shaderProgram, "Fade");
    GLint uniReflection = glGetUniformLocation(shaderProgram, "reflectionMultiple");


    glEnable(GL_DEPTH_TEST);

    auto t_start = std::chrono::high_resolution_clock::now();


    while(!glfwWindowShouldClose(window))
    {
        auto t_now = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

        glm::mat4 model;
        model = glm::rotate(model, time*glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

        glUniform1f(uniFade, (sin(0.5f * time) + 1.0f) / 2.0f);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        glClearColor(0.9f, 0.9f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // draw the cube
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        // draw the floor, writing to the stencil buffer in the process
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilMask(0xFF);
        glClear(GL_STENCIL_BUFFER_BIT);
 
        // don't write to the depth buffer so that the reflection still draws
        glDepthMask(GL_FALSE);
        glDrawArrays(GL_TRIANGLES, 36, 6);
        glDepthMask(GL_TRUE);
                
        // draw the reflected cube
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilMask(0x00);
        // attenuate the color
        glUniform3f(uniReflection, 0.3f, 0.3f, 0.3f);        
        model = glm::scale( glm::translate(model, glm::vec3(0, 0, -1.05)), glm::vec3(1, 1, -1));
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glUniform3f(uniReflection, 1.0f, 1.0f, 1.0f);
        glDisable(GL_STENCIL_TEST);
        
    }

    glfwTerminate();
}
