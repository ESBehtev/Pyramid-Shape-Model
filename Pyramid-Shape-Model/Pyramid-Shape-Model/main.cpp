//
//  main.cpp
//  Pyramid-Shape-Model
//
//  Created by Эдвард on 25.10.2024.
//

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Vertex coordinates and colors (for parallelogram)
GLfloat vertices[] = {
    // Vertex 1 (lower left)
    -2.0f,  0.0f, 0.0f,  // Position
     1.0f,  0.0f, 0.0f,  // Color (Red)
     0.0f,  0.0f, 1.0f,  // Normal

    // Vertex 2 (upper left)
    -1.0f,  3.0f, 0.0f,  // Position
     0.0f,  1.0f, 0.0f,  // Color (Green)
     0.0f,  0.0f, 1.0f,  // Normal

    // Vertex 3 (bottom right)
     2.0f,  0.0f, 0.0f,  // Position
     0.0f,  0.0f, 1.0f,  // Color (Blue)
     0.0f,  0.0f, 1.0f,  // Normal

    // Vertex 4 (upper right)
     3.0f,  3.0f, 0.0f,  // Position
     1.0f,  1.0f, 0.0f,  // Color (Yellow)
     0.0f,  0.0f, 1.0f,   // Normal
    
    // Вершина пирамиды (верх)
     0.0f,  0.0f, 5.0f,  // Позиция
     1.0f,  1.0f, 1.0f,  // Цвет (Белый)
     0.0f,  0.0f, 1.0f   // Нормаль
};

GLuint indices[] = {
    0, 1, 2,  // The first triangle
    1, 2, 3,   // The second triangle

    //  Pyramid
    0, 1, 4,
    1, 3, 4,
    3, 2, 4,
    2, 0, 4
};

// Camera position
glm::vec3 cameraPos   = glm::vec3(10.0f, 10.0f, 10.0f);
glm::vec3 cameraFront = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - cameraPos);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

// Adding variables for rotation angles
float rotationX = 0.0f;
float rotationY = 0.0f;

// Processing key input for shape rotation
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        rotationX -= 0.05f;  // Up
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        rotationX += 0.05f;  // Down
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        rotationY -= 0.05f;  // Left
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        rotationY += 0.05f;  // Right
}

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
out vec3 ourColor;
out vec3 fragNormal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
    fragNormal = aNormal;
})";

const char* fragmentShaderSource = R"(
#version 330 core
in vec3 ourColor;
in vec3 fragNormal;
out vec4 FragColor;
uniform vec3 lightDir;
void main()
{
    float ambient = 0.1;
    float diff = max(dot(normalize(fragNormal), normalize(lightDir)), 0.0);
    vec3 result = (ambient + diff) * ourColor;
    FragColor = vec4(result, 1.0);
})";

GLuint setupShaders(const char* vertexSrc, const char* fragmentSrc) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Pyramid", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    GLuint shaderProgram = setupShaders(vertexShaderSource, fragmentShaderSource);

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window)) {
           processInput(window);  // Input processing

           glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

           glUseProgram(shaderProgram);

           // Updating the model matrix for rotation
           glm::mat4 model = glm::mat4(1.0f);
           model = glm::rotate(model, rotationX, glm::vec3(1.0f, 0.0f, 0.0f));  // X-axis rotation
           model = glm::rotate(model, rotationY, glm::vec3(0.0f, 1.0f, 0.0f));  // Y-axis rotation

           glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);  // The camera view is unchanged
           int modelLoc = glGetUniformLocation(shaderProgram, "model");
           int viewLoc = glGetUniformLocation(shaderProgram, "view");
           int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
           int lightDirLoc = glGetUniformLocation(shaderProgram, "lightDir");

           glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
           glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
           glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
           glUniform3f(lightDirLoc, 1.0f, 1.0f, 1.0f);

           glBindVertexArray(VAO);
           glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

           glfwSwapBuffers(window);
           glfwPollEvents();
       }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
