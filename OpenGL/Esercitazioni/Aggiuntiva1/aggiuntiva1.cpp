#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.h>
#include <Camera.h>
#include <Model.h>

#include <iostream>

bool spotlightOn = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting info
glm::vec3 lightPos(1.5f, 0.5f, 1.0f);

// Rotation
float rotationRadius = 2.5f; // Raggio della circonferenza

// Velocità rotation
float speedFactor = 1.5f;

// shader path
char shader_vs_luce[] = "Esercitazioni/Aggiuntiva1/shaderlight.vs";
char shader_fs_luce[] = "Esercitazioni/Aggiuntiva1/shaderlight.fs";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("Esercitazioni/Aggiuntiva1/shader.vs", "Esercitazioni/Aggiuntiva1/shader.fs");
    Shader lightCubeShader(shader_vs_luce, shader_fs_luce);

    // load models
    // -----------
    Model ourModel("Texture/Vanguard/vanguard.obj");

    float vertices[] = {
        // positions         
       -0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f, -0.5f,  
        0.5f,  0.5f, -0.5f,  
        0.5f,  0.5f, -0.5f,  
       -0.5f,  0.5f, -0.5f, 
       -0.5f, -0.5f, -0.5f,

       -0.5f, -0.5f,  0.5f, 
        0.5f, -0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f, 
        0.5f,  0.5f,  0.5f,  
       -0.5f,  0.5f,  0.5f,  
       -0.5f, -0.5f,  0.5f,  

       -0.5f,  0.5f,  0.5f, 
       -0.5f,  0.5f, -0.5f, 
       -0.5f, -0.5f, -0.5f,
       -0.5f, -0.5f, -0.5f,
       -0.5f, -0.5f,  0.5f, 
       -0.5f,  0.5f,  0.5f,

        0.5f,  0.5f,  0.5f, 
        0.5f,  0.5f, -0.5f,  
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f,  0.5f, 
        0.5f,  0.5f,  0.5f, 

       -0.5f, -0.5f, -0.5f, 
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f,  0.5f, 
        0.5f, -0.5f,  0.5f,  
       -0.5f, -0.5f,  0.5f, 
       -0.5f, -0.5f, -0.5f,  

       -0.5f,  0.5f, -0.5f,  
        0.5f,  0.5f, -0.5f,  
        0.5f,  0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f, 
       -0.5f,  0.5f,  0.5f, 
       -0.5f,  0.5f, -0.5f, 
    };

    // Configure light VAO (and VBO)
    unsigned int VBO, lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(lightCubeVAO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // calcola la rotazione della luce
        lightPos.x = sin(glfwGetTime() * speedFactor) * rotationRadius;
        lightPos.z = cos(glfwGetTime() * speedFactor) * rotationRadius;

        // calcola la rotazione della telecamera
        camera.Position.x = sin(glfwGetTime() * speedFactor) * rotationRadius;
        camera.Position.z = cos(glfwGetTime() * speedFactor) * rotationRadius;

        // imposta la vista della telecamera in modo che guardi il modello al centro
        camera.Front = glm::normalize(-camera.Position);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // set light properties
        // Posizione dell'osservatore
        ourShader.setVec3("viewPos", camera.Position);
        // lightPos.x = static_cast<float>(sin(glfwGetTime()));
        // Posizione della luce
        ourShader.setVec3("lightPos", lightPos);

        // Directional light
        ourShader.setVec3("lightDirection", -0.2f, -1.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
        ourShader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
        ourShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

        // Point light 
        ourShader.setVec3("lightPos", lightPos);
        ourShader.setVec3("pointLights[0].ambient", 0.1f, 0.1f, 0.1f);
        ourShader.setVec3("pointLights[0].diffuse", 0.5f, 0.5f, 0.5f);
        ourShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[0].constant", 1.0f);
        ourShader.setFloat("pointLights[0].linear", 0.09f);
        ourShader.setFloat("pointLights[0].quadratic", 0.032f);

        // SpotLight
        ourShader.setVec3("lightPosSpotLight", camera.Position);
        ourShader.setVec3("lightDirectionSpotLight", camera.Front);
        ourShader.setFloat("spotLight.constant", 1.0f);
        ourShader.setFloat("spotLight.linear", 0.009f);
        ourShader.setFloat("spotLight.quadratic", 0.0032f);
        ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(2.0f)));
        ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(3.0f)));

        if (spotlightOn)
        {
            ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
            ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        }
        else
        {
            ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            ourShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
            ourShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
        }

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f)); 
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); 
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        // draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightCubeShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    static bool eKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        if (!eKeyPressed)
        {
            spotlightOn = !spotlightOn;
            eKeyPressed = true;
        }
    }
    else
    {
        eKeyPressed = false;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}