//OpenGL includes
#include <GLFW/glfw3.h>
#include <glad/include/glad/glad.h>
#include <glad/include/KHR/khrplatform.h>

//GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Freetype includes
#include <freetype-2.9.1/include/ft2build.h>
#include FT_FREETYPE_H

//VS includes
#include <iostream>
#include <vector>
#include <string>
#include <map>

//Internal includes
#include "Shader.h"
#include "Texture2D.h"
#include "Camera.h"
#include "Model.h"

//Callbacks and Functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
int shipMovement(GLFWwindow* window);
int updatePlanetCam(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void setStaticLights(Shader shader);
void setSpotlight(Shader shader);
void RenderText(Shader shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

//Window settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//Global variables
float mixValue = 0.2f;

//Camera
//Positions             x     y     z
Camera camera(glm::vec3(0.0f, 50.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool InitialMouseMovement = true;
Camera tempCam;
glm::mat4 tempMat;

//DeltaTime
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//Light Variables
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

bool space = true;

float skyboxVertices[] = {
    // positions          
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

//Skybox Faces
std::vector<std::string> faces
{
    "Images/bkg/blue/bkg1_right.png",
    "Images/bkg/blue/bkg1_left.png",
    "Images/bkg/blue/bkg1_top.png",
    "Images/bkg/blue/bkg1_back.png",
    "Images/bkg/blue/bkg1_front.png",
    "Images/bkg/blue/bkg1_back.png",
};

//Model Matrices
glm::mat4 model4 = glm::mat4(1.0f);
glm::mat4 model5 = glm::mat4(1.0);
glm::mat4 model6 = glm::mat4(1.0);
glm::mat4 model7 = glm::mat4(1.0f);
glm::mat4 model8 = glm::mat4(1.0f);
glm::mat4 model9 = glm::mat4(1.0f);

//Character Struct
struct Character
{
    GLuint TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    GLuint Advance;
};
std::map<GLchar, Character> Characters;
GLuint textVAO, textVBO;


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Window", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    //Callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //Capture Mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Load Glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //Freetype
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        std::cout << "Failed to initialize FreeType" << std::endl;

    //Load Font
    FT_Face face;
    if (FT_New_Face(ft, "Fonts/Exan-Regular.ttf", 0, &face))
        std::cout << "Failed to load font" << std::endl;

    //Set Size
    FT_Set_Pixel_Sizes(face,0, 48);

    //Load Characters (128)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //Removes alignment restriction

    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;

    for (GLubyte c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "Failed to load char" << std::endl;
            continue;
        }

        //Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //Store in vector
        Character character = { texture, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows), glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top), face->glyph->advance.x };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

   
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);



    glm::mat4 textProjection = glm::ortho(0.0f, static_cast<GLfloat>(SCR_WIDTH), 0.0f, static_cast<GLfloat>(SCR_HEIGHT));

    //States
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);

    //Skybox Array
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   
    //Skybox Data
    Texture2D* skyboxTexture = new Texture2D();
    skyboxTexture->LoadCubeMap(faces);

    //Shaders
    Shader gasShader("Shaders/Model2.vs", "Shaders/Model2.fs");
    Shader modelShader("Shaders/Model2.vs", "Shaders/Model2.fs");
    Shader lightModelShader("Shaders/model.vs", "Shaders/model.fs");
    Shader skyboxShader("Shaders/Skybox.vs", "Shaders/Skybox.fs");
    Shader earthShader("Shaders/Model2.vs", "Shaders/Model2.fs");
    Shader redShader("Shaders/Model2.vs", "Shaders/Model2.fs");
    Shader alienShader("Shaders/Model2.vs", "Shaders/Model2.fs");
    Shader sednaShader("Shaders/Model2.vs", "Shaders/Model2.fs");
    Shader textShader("Shaders/Text.vs", "Shaders/Text.fs");
    Shader asteroidShader("Shaders/Asteroid.vs", "Shaders/Asteroid.fs");
    Shader asteroidPlanetShader("Shaders/model.vs", "Shaders/model.fs");


    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    //Models
    Model sunModel((char*)"Models/planet/planet.obj");
    Model starDestroyerModel((char*)"Models/Star_Destroyer/star_destroyer.obj");
    Model gasModel((char*)"Models/gas planet/planet.obj");
    Model earthModel((char*)"Models/earth/planet.obj");
    Model redModel((char*)"Models/red/planet.obj");
    Model alienModel((char*)"Models/alien/planet.obj");
    Model sednaModel((char*)"Models/sedna/planet.obj");
    Model asteroidModel((char*)"Models/rock/rock.obj");
    Model iceModel((char*)"Models/ice planet/planet.obj");

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //Text
    textShader.use();
    textShader.setMat4("projection", textProjection);

    //Sun
    lightModelShader.use();
    glm::mat4 model3 = glm::mat4(1.0f);
    model3 = glm::scale(model3, glm::vec3(200.0f, 200.0f, 200.0f));
    lightModelShader.setMat4("model", model3);

    //Ship
    modelShader.use();
    glm::mat4 model2 = glm::mat4(1.0f);
    model2 = glm::translate(model2, glm::vec3(0.0f, -1.75f, 500.0f)); 
    model2 = glm::scale(model2, glm::vec3(0.2f, 0.2f, 0.2f));
    modelShader.setMat4("model", model2);

    glm::vec3 shipCenter = glm::vec3(0.0f, -1.75f, 0.0f);
    float shipRotation = 0.0f;

    //Gas Planet
    gasShader.use();
    model4 = glm::scale(model4, glm::vec3(70.0f, 70.0f, 70.0f));
    model4 = glm::translate(model4, glm::vec3(-100.0f, 0.0f, -100.0f));
    gasShader.setMat4("model", model4);

    //Earth
    earthShader.use();
    model5 = glm::scale(model5, glm::vec3(80.0f, 80.0f, 80.0f));
    model5 = glm::rotate(model5, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model5 = glm::translate(model5, glm::vec3(-200.0f, 200.0f, 0.0f));
    earthShader.setMat4("model", model5);

    //Red Planet
    redShader.use();
    model6 = glm::scale(model6, glm::vec3(50.0f, 50.0f, 50.0f));
    model6 = glm::rotate(model6, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model6 = glm::translate(model6, glm::vec3(-350.0f, 350.0f, 0.0f));
    redShader.setMat4("model", model6);

    //Alien Planet
    alienShader.use();
    model7 = glm::scale(model7, glm::vec3(90.0f, 90.0f, 90.0f));
    model7 = glm::translate(model7, glm::vec3(-500.0f, 0.0f, 400.0f));
    alienShader.setMat4("model", model7);

    //Sedna Planet
    sednaShader.use();
    model8 = glm::scale(model8, glm::vec3(75.0f, 75.0f, 75.0f));
    model8 = glm::translate(model8, glm::vec3(-650.0f, 0.0f, -500.0f));
    sednaShader.setMat4("model", model8);

    //Asteroid and Planet
    //Planet
    asteroidPlanetShader.use();
    model9 = glm::scale(model9, glm::vec3(250.0f, 250.0f, 250.0f));
    //model9 = glm::translate(model9, glm::vec3(384.0f, 0.0f, -80.0f));
    asteroidPlanetShader.setMat4("model", model9);


    //Asteroids
    unsigned int asteroidNum = 50000;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[asteroidNum];
    srand(glfwGetTime());
    float radius = 150.0f;
    float offset = 25.0f;
    for (unsigned int i = 0; i < asteroidNum; i++)
    {
        glm::mat4 model10 = glm::mat4(1.0f);
        model10 = glm::scale(model10, glm::vec3(80.0f, 80.0f, 80.0f));
        //model10 = glm::translate(model10, glm::vec3(1200.0f, 0.0f, -250.0f));
        
        //Displacement
        float angle = (float)i / (float)asteroidNum * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model10 = glm::translate(model10, glm::vec3(x, y, z));

        //Scale
        float scale = (rand() % 20) / 100.0f + 0.05f;
        model10 = glm::scale(model10, glm::vec3(scale));

        ////Rotation
        //float rotAngle = (rand() % 360);
        //model10 = glm::rotate(model10, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        //Add to matrix array
        modelMatrices[i] = model10;
    }

    //Asteroid Instance Array
    unsigned int asteroidBuffer;
    glGenBuffers(1, &asteroidBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, asteroidBuffer);
    glBufferData(GL_ARRAY_BUFFER, asteroidNum * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    for (unsigned int i = 0; i < asteroidModel.meshes.size(); i++)
    {
        unsigned int asteroidVAO = asteroidModel.meshes[i].VAO;
        glBindVertexArray(asteroidVAO);

        //Vertex Shader Attributes
        glEnableVertexAttribArray(3); //Location 3
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    while (!glfwWindowShouldClose(window))
    {
        //Calculate frame
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //Process Input
        processInput(window);

        //Clear Things
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Perspective elements            Field of View         Aspect ratio (Width:Height)          Zc    Zf
        glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200000.0f);

        //Set camera view
        glm::mat4 view = camera.GetViewMatrix();

        //World transform
        glm::mat4 model = glm::mat4(1.0f);



        if (space)
        {
            //Light
            lightModelShader.use();
            lightModelShader.setVec3("viewPos", camera.Position);

            //Sun Model
            lightModelShader.setMat4("projection", proj);
            view = camera.GetViewMatrix();
            lightModelShader.setMat4("view", view);
            model3 = glm::rotate(model3, glm::radians(15 * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
            lightModelShader.setMat4("model", model3);
            sunModel.Draw(lightModelShader);


            //Model
            modelShader.use();
            modelShader.setFloat("material.shininess", 32.0f);

            //Model
            modelShader.setMat4("projection", proj);

            if (shipMovement(window) == 1) //Move Forwards
            {
                shipRotation = 0.0f;
                model2 = glm::translate(model2, glm::vec3(5000 * deltaTime, 0.0f, 0.0f));
                camera.Position.x = model2[3].x;
                camera.Position.y = model[3].y + 30.0f;
                camera.Position.z = model2[3].z;
                glEnable(GL_BLEND);

            }
            else if (shipMovement(window) == 2) //Move Backwards
            {
                shipRotation = 0.0f;
                model2 = glm::translate(model2, glm::vec3(-(5000 * deltaTime), 0.0f, 0.0f));

                camera.Position.x = model2[3].x;
                camera.Position.y = 30.0f;
                camera.Position.z = model2[3].z;
            }
            else if (shipMovement(window) == 3) //Rotate Left
            {
                //model2 = glm::translate(model2, glm::vec3(0.0f, 0.0f, -(5000 * deltaTime)));
                //model4 = glm::rotate(model4, glm::radians(25 * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
                if (shipRotation < 45.0f)
                {
                    shipRotation += 1.0f * deltaTime;
                    model2 = glm::rotate(model2, glm::radians(shipRotation * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
                }
                else if (shipRotation > 45.0f)
                {
                    shipRotation = 45.0f;
                }


                //camera.Position.x = model2[3].x;
                //camera.Position.y = 30.0f;
                //camera.Position.z = model2[3].z;
            }
            else if (shipMovement(window) == 4) //Rotate Right
            {
                if (shipRotation > -45.0f)
                {
                    shipRotation -= 1.0f * deltaTime;
                    model2 = glm::rotate(model2, glm::radians(shipRotation * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
                }
                else if (shipRotation < -45.0f)
                {
                    shipRotation = -45.0f;
                }
                camera.Position.x = model2[3].x;
                camera.Position.y = 30.0f;
                camera.Position.z = model2[3].z;
            }
            else //Gradual decrease
            {
                if (shipRotation < -1 && shipRotation != 0.0f)
                {
                    shipRotation += 1.0f * deltaTime;
                    //std::cout << "Adding" << std::endl;
                }
                else if (shipRotation > 1 && shipRotation != 0.0f)
                {
                    shipRotation -= 1.0f * deltaTime;
                    //std::cout << "Subtracting" << std::endl;
                }
                else if (shipRotation < 1 && shipRotation > -1)
                {
                    shipRotation = 0.0f;
                }
                model2 = glm::rotate(model2, glm::radians(shipRotation * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
            }

            modelShader.setVec3("viewPos", camera.Position);
            setStaticLights(modelShader);
            setSpotlight(modelShader);
            view = camera.GetViewMatrix();
            modelShader.setMat4("view", view);
            modelShader.setMat4("model", model2);
            starDestroyerModel.Draw(modelShader);


            //Gas Model
            gasShader.use();
            gasShader.setFloat("material.shininess", 32.0f);
            gasShader.setVec3("viewPos", camera.Position);

            setStaticLights(gasShader);
            setSpotlight(gasShader);

            //Gas Model
            gasShader.setMat4("projection", proj);
            model4 = glm::translate(model4, glm::vec3(100.0f, 0.0, 100.0f));
            model4 = glm::rotate(model4, glm::radians(25 * deltaTime), glm::vec3(0.0f,1.0f, 0.0f));
            model4 = glm::translate(model4, glm::vec3(-100.0f, 0.0f, -100.0f));



            view = camera.GetViewMatrix();
            gasShader.setMat4("view", view);
            gasShader.setMat4("model", model4);
            gasModel.Draw(gasShader);

            //Earth Model
            earthShader.use();
            earthShader.setFloat("material.shininess", 32.0f);
            earthShader.setVec3("viewPos", camera.Position);

            setStaticLights(earthShader);
            setSpotlight(earthShader);

            //Earth Model
            earthShader.setMat4("projection", proj);
            view = camera.GetViewMatrix();
            earthShader.setMat4("view", view);
            //model5 = glm::translate(model5, glm::vec3(-300.0f, 300.0f, 0.0f));
            model5 = glm::translate(model5, glm::vec3(200.0f, -200.0f, 0.0f));
            model5 = glm::rotate(model5, glm::radians(20 * deltaTime), glm::vec3(0.0f, 0.0f, -1.0f));
            model5 = glm::translate(model5, glm::vec3(-200.0f, 200.0f, 0.0f));

            earthShader.setMat4("model", model5);
            earthModel.Draw(earthShader);

            //Red Model
            redShader.use();
            redShader.setFloat("material.shininess", 32.0f);
            redShader.setVec3("viewPos", camera.Position);

            setStaticLights(redShader);
            setSpotlight(redShader);

            //Red Model
            redShader.setMat4("projection", proj);
            view = camera.GetViewMatrix();
            redShader.setMat4("view", view);
            //model5 = glm::translate(model5, glm::vec3(-350.0f, 350.0f, 0.0f));
            model6 = glm::translate(model6, glm::vec3(350.0f, -350.0f, 0.0f));
            model6 = glm::rotate(model6, glm::radians(15 * deltaTime), glm::vec3(0.0f, 0.0f, -1.0f));
            model6 = glm::translate(model6, glm::vec3(-350.0f, 350.0f, 0.0f));
            redShader.setMat4("model", model6);
            redModel.Draw(redShader);

            //Alien Model
            alienShader.use();
            alienShader.setFloat("material.shininess", 32.0f);
            alienShader.setVec3("viewPos", camera.Position);

            setStaticLights(alienShader);
            setSpotlight(alienShader);

            //Alien Model
            alienShader.setMat4("projection", proj);
            view = camera.GetViewMatrix();
            alienShader.setMat4("view", view);
            model7 = glm::translate(model7, glm::vec3(500.0f, 0.0f, -400.0f));
            model7 = glm::rotate(model7, glm::radians(10 * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
            model7 = glm::translate(model7, glm::vec3(-500.0f, 0.0f, 400.0f));
            alienShader.setMat4("model", model7);
            alienModel.Draw(alienShader);

            //Sedna Model
            sednaShader.use();
            sednaShader.setFloat("material.shininess", 32.0f);
            sednaShader.setVec3("viewPos", camera.Position);

            setStaticLights(sednaShader);
            setSpotlight(sednaShader);

            //Sedna Model
            sednaShader.setMat4("projection", proj);
            view = camera.GetViewMatrix();
            sednaShader.setMat4("view", view);
            model8 = glm::translate(model8, glm::vec3(650.0f, 0.0f, 500.0f));
            model8 = glm::rotate(model8, glm::radians(5 * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
            model8 = glm::translate(model8, glm::vec3(-650.0f, 0.0f, -500.0f));
            sednaShader.setMat4("model", model8);
            sednaModel.Draw(sednaShader);


            //Always draw last
            glDepthFunc(GL_LEQUAL);  //Make sure skybox doesn't overwrite objects
            skyboxShader.use();
            glm::mat4 view2 = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
            skyboxShader.setMat4("view", view2);
            skyboxShader.setMat4("projection", proj);
            //Skybox Cube
            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture->GetID());
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            glDepthFunc(GL_LESS); //Set back to usual mode for other objects



            //Render Text
            if (updatePlanetCam(window) == 1)
            {
                RenderText(textShader, "Centra", 10.0f, 550.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            }
            else if (updatePlanetCam(window) == 2)
            {
                RenderText(textShader, "Gaia Primus", 10.0f, 550.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            }
            else if (updatePlanetCam(window) == 3)
            {
                RenderText(textShader, "Septum", 10.0f, 550.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            }
            else if (updatePlanetCam(window) == 4)
            {
                RenderText(textShader, "Chadus Prime", 10.0f, 550.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            }
            else if (updatePlanetCam(window) == 5)
            {
                RenderText(textShader, "Ignis", 10.0f, 550.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            }
            //RenderText(textShader, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
            //RenderText(textShader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

            std::string shipPosition = "Star Destroyer Coordinates X " + std::to_string(int(model2[3].x)) + " Y" + std::to_string(int(model2[3].y)) + " Z " + std::to_string(int(model2[3].z));

            if ((model2[3].x < 0) && (model2[3].z < 0))
            {
                shipPosition = "Star Destroyer Coordinates X M" + std::to_string(int(model2[3].x)) + " Y " + std::to_string(int(model2[3].y)) + " Z M" + std::to_string(int(model2[3].z));
            }
            else if (model2[3].x < 0)
            {
                shipPosition = "Star Destroyer Coordinates X M" + std::to_string(int(model2[3].x)) + " Y " + std::to_string(int(model2[3].y)) + " Z " + std::to_string(int(model2[3].z));
            }
            else if (model2[3].z < 0)
            {
                shipPosition = "Star Destroyer Coordinates X " + std::to_string(int(model2[3].x)) + " Y " + std::to_string(int(model2[3].y)) + " Z M" + std::to_string(int(model2[3].z));
            }

            if (shipMovement(window) != 0) //Move Forwards
            {
                RenderText(textShader, shipPosition, 10.0f, 550.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            }
        }
        else
        {
            //Planet and Asteroid
            asteroidShader.use();
            asteroidShader.setMat4("projection", proj);
            view = camera.GetViewMatrix();
            asteroidShader.setMat4("view", view);

            asteroidPlanetShader.use();
            asteroidPlanetShader.setMat4("projection", proj);
            view = camera.GetViewMatrix();
            asteroidPlanetShader.setMat4("view", view);

            // draw planet
            model9 = glm::rotate(model9, glm::radians(15 * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
            asteroidPlanetShader.setMat4("model", model9);
            iceModel.Draw(asteroidPlanetShader);

            // draw meteorites
            asteroidShader.use();
            asteroidShader.setInt("texture_diffuse1", 0);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, asteroidModel.textures_loaded[0].id);
            for (unsigned int i = 0; i < asteroidModel.meshes.size(); i++)
            {
                glBindVertexArray(asteroidModel.meshes[i].VAO);
                glDrawElementsInstanced(GL_TRIANGLES, asteroidModel.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, asteroidNum);
                glBindVertexArray(0);
            }

     

            //Always draw last
            glDepthFunc(GL_LEQUAL);  //Make sure skybox doesn't overwrite objects
            skyboxShader.use();
            glm::mat4 view2 = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
            skyboxShader.setMat4("view", view2);
            skyboxShader.setMat4("projection", proj);
            //Skybox Cube
            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture->GetID());
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            glDepthFunc(GL_LESS); //Set back to usual mode for other objects
        }
        int time = glfwGetTime();
        RenderText(textShader, "Elapsed time " + std::to_string(time), 10.0f, 10.0F, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

    
//Keyboard Callback
void processInput(GLFWwindow* window)
{
    //Opacity
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //Movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        if (space)
        {
            space = false;
        }
        else if(!space)
        {
            space = true;

        }
    }

}



//Resize Window Callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//Mouse Movement Callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (InitialMouseMovement)
    {
        lastX = xpos;
        lastY = ypos;
        InitialMouseMovement = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; //Y-axis reversed

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double offset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

int shipMovement(GLFWwindow* window)
{
    //Ship Movement
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        return 1;
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        return 2;
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        return 3;
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        return 4;
    else
        return 0;
}

void setStaticLights(Shader shader)
{
    shader.setVec3("pointLights[0].position", glm::vec3(0.00f, 1.75f, 200.0f));
    shader.setVec3("pointLights[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
    shader.setVec3("pointLights[0].diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setFloat("pointLights[0].constant", 1.0f);
    shader.setFloat("pointLights[0].linear", 0.000000000014);
    shader.setFloat("pointLights[0].quadratic", 0.00000000000007);
}

void setSpotlight(Shader shader)
{
    shader.setVec3("spotLight.position", camera.Position);
    shader.setVec3("spotLight.direction", camera.Front);
    shader.setVec3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
    shader.setVec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setFloat("spotLight.constant", 1.0f);
    shader.setFloat("spotLight.linear", 0.000007);
    shader.setFloat("spotLight.quadratic", 0.0000002);
    shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
}

void RenderText(Shader shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    shader.use();
    shader.setVec3("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    //Iterate through characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

        //Update VBO
        GLfloat vertices[6][4] = {
            {xpos, ypos + h, 0.0f, 0.0f},
            {xpos, ypos, 0.0f, 1.0f},
            {xpos + w, ypos, 1.0f, 1.0f},

            {xpos, ypos + h, 0.0f, 0.0f},
            {xpos + w, ypos, 1.0f, 1.0f},
            {xpos + w, ypos + h, 1.0f, 0.0f}
        };

        glDisable(GL_DEPTH_TEST);

        //Render Glyph
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        //Update VBO Memory
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //Advance to next glyph
        //Bitshift by 6
        x += (ch.Advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_DEPTH_TEST);
}

int updatePlanetCam(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        //camera.Position = glm::vec3(-6500.0f, 0.0f, -6500.0f);
        camera.Position.x = model4[3].x;
        camera.Position.y = model4[3].y + 500.0f;
        camera.Position.z = model4[3].z - 200.0f;
        return 1;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        camera.Position.x = model5[3].x;
        camera.Position.y = model5[3].y + 500.0f;
        camera.Position.z = model5[3].z - 200.0f;
        return 2;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        camera.Position.x = model6[3].x;
        camera.Position.y = model6[3].y + 500.0f;
        camera.Position.z = model6[3].z - 200.0f;
        return 3;
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        camera.Position.x = model7[3].x;
        camera.Position.y = model7[3].y + 500.0f;
        camera.Position.z = model7[3].z - 200.0f;
        return 4;
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        camera.Position.x = model8[3].x;
        camera.Position.y = model8[3].y + 500.0f;
        camera.Position.z = model8[3].z - 200.0f;
        return 5;
    }
}