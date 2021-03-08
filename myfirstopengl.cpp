#include<iostream>
#include<math.h>
#include<vector>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/transform.hpp>

#include<Shader.h>
#include<Camera.h>

using namespace std;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();
// Window dimensions
const GLuint WIDTH = 1600, HEIGHT = 1200; // 窗口大小

Camera  camera(glm::vec3(0.0f, 0.0f, 20.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool    keys[1024];
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame


// The MAIN function, from here we start the application and run the game loop
int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();

    // Define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glEnable(GL_DEPTH_TEST);

    Shader ourShader("./Default.vert", "./Default.frag");
    Shader lightShader("./light.vert", "./light.frag");

    vector<GLfloat> vertices;
    GLfloat temp = 0.0f;

    GLfloat j_max = 60, i_max = 60;
    GLint len = 6;
    for (int j = 0; j <= j_max; j++)
    {
        GLfloat radius = 1.0f;

        GLfloat alpha = (2 * 3.1415926 / j_max);


        for (int i = 0; i <= i_max; i++)
        {
            GLfloat theta = (2 * 3.1415926 / i_max);
            GLfloat x = radius * sinf(i * theta);
            GLfloat y = radius * cosf(i * theta) * sinf(j * alpha);
            GLfloat z = radius * cosf(i * theta) * cosf(j * alpha);

            GLfloat red = 0.0f, green = 0.0f, blue = 0.0f;
            red = (i + j) % 3 == 0 ? 1.0f : 0.5f;
            green = (i + j) % 3 == 1 ? 1.0f : 0.5f;
            blue = (i + j) % 3 == 2 ? 1.0f : 0.5f;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(red);
            vertices.push_back(green);
            vertices.push_back(blue);
        }
    }
    //顶点生成↑-------------------------------------

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, len * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    //位置属性↑-------------------------------------------------

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, len * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    //颜色属性↑------------------------------------------------

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // 只需要绑定VBO不用再次设置VBO的数据，因为容器(物体)的VBO数据中已经包含了正确的立方体顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 设置灯立方体的顶点属性指针(仅设置灯的顶点数据)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, len * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, len * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //时间设定↑------------------------------
        glfwPollEvents();
        do_movement();

        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw our first triangle
        lightShader.Use();
        glm::mat4 view;
        view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
        glm::mat4 transform = glm::mat4(1.0);
        //transform = glm::rotate(transform, (GLfloat)glfwGetTime(), glm::vec3(0.0f, 0.0f, -1.0f));
        //transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        GLint transformLoc = glGetUniformLocation(lightShader.Program, "transform");
        GLint projLoc = glGetUniformLocation(lightShader.Program, "projection");
        GLint viewLoc = glGetUniformLocation(lightShader.Program, "view");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glBindVertexArray(lightVAO);
        int depth = 15;
        for (int i = 0; i < depth; i++)
        {
            transform = glm::mat4(1.0);
            transform = glm::rotate(transform, glm::radians(1.0f * i), glm::vec3(0.0f, 0.0f, 1.0f));
            transform = glm::rotate(transform, (GLfloat)glfwGetTime(), glm::vec3(0.0f, 0.0f, -1.0f));
            transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glDrawArrays(GL_LINE_LOOP, 0, vertices.size() / len);
        }
        glBindVertexArray(0);
        
        ourShader.Use();

        float scale_num[] = { 0.05f,0.12f,0.12f,0.06f,0.35f,0.30f,0.12f,0.12f };
        float translate_length[] = { 1.08f,1.52f,2.8f,4.0f,6.3f,9.6f,13.6f,16.0f };
        float rotate_speed[] = { 4.2f,1.5f,1.0f,0.8f,0.6f,0.5f,0.36f,0.3f };


        glBindVertexArray(VAO);
        transformLoc = glGetUniformLocation(ourShader.Program, "transform");
        projLoc = glGetUniformLocation(ourShader.Program, "projection");
        viewLoc = glGetUniformLocation(ourShader.Program, "view");

        GLint lightColorLoc = glGetUniformLocation(ourShader.Program, "lightColor");
        GLint lightPosLoc = glGetUniformLocation(ourShader.Program, "lightPos");
        GLint viewPosLoc = glGetUniformLocation(ourShader.Program, "viewPos");
        glUniform3f(lightColorLoc, 1.0f, 0.8f, 0.8f);
        glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);


        for (int i = 0; i < 8; i++)
        {
            transform = glm::mat4(1.0);
            transform = glm::rotate(transform, (GLfloat)glfwGetTime()*rotate_speed[i], glm::vec3(0.0f, 0.0f, 1.0f));
            transform = glm::translate(transform, glm::vec3(2.0f * translate_length[i], 0.0f, 0.0f));
            transform = glm::rotate(transform, (GLfloat)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
            transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            transform = glm::scale(transform, glm::vec3(1.5*scale_num[i],1.5*scale_num[i],1.5*scale_num[i]));
            GLint transformLoc = glGetUniformLocation(ourShader.Program, "transform");
            GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");
            GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

            glDrawArrays(GL_LINE_LOOP, 0, vertices.size() / len);
            if (i == 2)
            {
                transform = glm::mat4(1.0);
                transform = glm::rotate(transform, (GLfloat)glfwGetTime() * rotate_speed[i], glm::vec3(0.0f, 0.0f, 1.0f));
                transform = glm::translate(transform, glm::vec3(2.0f * translate_length[i], 0.0f, 0.0f));
                transform = glm::rotate(transform, (GLfloat)glfwGetTime() * rotate_speed[i] * 2, glm::vec3(0.0f, 0.0f, 1.0f));
                transform = glm::translate(transform, glm::vec3(0.5f, 0.0f, 0.0f));
                transform = glm::rotate(transform, (GLfloat)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
                transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                transform = glm::scale(transform, glm::vec3(0.5 * scale_num[i], 0.5 * scale_num[i], 0.5 * scale_num[i]));
                GLint transformLoc = glGetUniformLocation(ourShader.Program, "transform");
                GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");
                GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
                glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
                glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

                glDrawArrays(GL_LINE_LOOP, 0, vertices.size() / len);
            }
        }
        glBindVertexArray(0);
        
        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void do_movement()
{
    // Camera controls
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


/*
    制作人：杨君涵
    参考资料：LearnOpenGL-CN，https://learnopengl-cn.readthedocs.io/zh/latest/
*/
