#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string.h>
#include <cmath>
#include <vector>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonValues.h"

#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Texture.h"
#include "MusicProcessing.h"
#include "Material.h"

#include "DirectionalLight.h"
#include "PointLight.h"

const float toRadians = 3.14159265f / 180.0f;
const int barNum = 10;

Window mainWindow;


std::vector<Mesh*> meshList;

std::vector<Shader*> shaderList;
Camera camera;

Texture cubeTexture;
Texture dirtTexture;


Material material;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];

static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";

void CreateObjects()
{
    GLfloat floorVertices[] = {
    -10.0f, 0.0f, -10.0f,   0.0f, 0.0f,     0.0f, -1.0f, 0.0f,
    10.0f, 0.0f, -10.0f,    10.0f, 0.0f,    0.0f, -1.0f, 0.0f,
    -10.0f, 0.0f, 10.0f,    0.0f, 10.0f,    0.0f, -1.0f, 0.0f,
    10.0f, 0.0f, 10.0f,     10.0f, 10.0f,   0.0f, -1.0f, 0.0f
    };

    unsigned int floorIndices[] = {
        0, 2, 1,
        1, 2, 3
    };

    Mesh* floor = new Mesh();
    floor->CreateMesh(floorVertices, floorIndices, 32, 6);
    meshList.push_back(floor);

    unsigned int indices[36] = {
        // front
        0, 1, 2,
        2, 3, 0,
        // right
        1, 5, 6,
        6, 2, 1,
        // back
        7, 6, 5,
        5, 4, 7,
        // left
        4, 0, 3,
        3, 7, 4,
        // bottom
        4, 5, 1,
        1, 0, 4,
        // top
        3, 2, 6,
        6, 7, 3
    };

    GLfloat vertices[64] = {
        //  x      y     z     u     v          normals

        //front face
         -1.0f, -1.0f, 1.0f,    0.0f, 0.0f,     0.0f, 0.0f, 0.0f,  //0
          1.0f, -1.0f, 1.0f,    1.0f, 0.0f,     0.0f, 0.0f, 0.0f,  //1
          1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     0.0f, 0.0f, 0.0f,  //2  
          -1.0f, 1.0f, 1.0f,    0.0f, 1.0f,     0.0f, 0.0f, 0.0f,  //3

        //back face
        -1.0f, -1.0f, -1.0f,    0.0f, 0.0f,     0.0f, 0.0f, 0.0f,  //4
         1.0f, -1.0f, -1.0f,    1.0f, 0.0f,     0.0f, 0.0f, 0.0f,  //5
         1.0f, 1.0f, -1.0f,     1.0f, 1.0f,     0.0f, 0.0f, 0.0f,  //6
         -1.0f, 1.0f, -1.0f,    0.0f, 1.0f,     0.0f, 0.0f, 0.0f   //7
    };

    for (int i = 0; i < barNum; i++)
    {
        Mesh* cubes[barNum];
        cubes[i] = new Mesh();
        cubes[i]->CreateMesh(vertices, indices, 36, 64);
        cubes[i]-> CalcAverageNormals(indices, 36, vertices, 64, 8, 5);
        meshList.push_back(cubes[i]);
    }
}

void CreateShaders()
{
    Shader* shader1 = new Shader();
    shader1->CreateFromFile(vShader, fShader);
    shaderList.push_back(shader1);
}


int main()
{
    mainWindow = Window();
    mainWindow.Initialise();

    CreateObjects(); 
    CreateShaders();

    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.5f, 0.5f);
    
    cubeTexture = Texture((char*)("textures/green2.png"));
    cubeTexture.LoadTexture();
    dirtTexture = Texture((char*)("textures/dirt.png"));
    dirtTexture.LoadTexture();

    material = Material(1.0f, 32); // reflectivity for specular lighting

    mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 
                                 0.1f, 0.3f, //ambient intensity and diffuse intensity
                                 0.0f, 1.0f, 3.0f);
    


    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
        uniformSpecularIntensity = 0, uniformShininess = 0;

    MusicProcessing player;
    player.loadMusic("sample.wav");
    player.playMusic();

    // initialize spectrum
    int spectrumSize = player.initSpectrum(12);
    float* frequencies = new float[spectrumSize]();

   
    GLfloat targetPos = 0.0f;

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

    while (!mainWindow.getShouldClose())
    {
        // Get and handle user input events
        glfwPollEvents();

        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;
        player.getFrequency(frequencies);


        camera.keyControl(mainWindow.getKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

        //set background colour

        float red = 0.0f + (frequencies[2] * 0.5f);
        float green = 0.0f + (frequencies[2] * 0.5f);
        float blue = 0.2f + (frequencies[2] * 0.5f);

        glClearColor(red, green, blue, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pointLights[0] = PointLight(frequencies[2] * 2, 0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, -5.0f, -20.0f,
            0.3, frequencies[6], 0.1f);
     
        shaderList[0]->UseShader();
        uniformModel = shaderList[0]->GetModelLocation();
        uniformProjection = shaderList[0]->GetProjectionLocation();
        uniformView = shaderList[0]->GetViewLocation();
        targetPos = shaderList[0]->GetTargetLocation();
        uniformEyePosition = shaderList[0]->GetEyePositionLocation();
        uniformSpecularIntensity = shaderList[0]->GetSpecularIntensityLocation();
        uniformShininess = shaderList[0]->GetShininessLocation();


        shaderList[0]->SetDirectionalLight(&mainLight);
        shaderList[0]->SetPointLights(pointLights, 1);

        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
        
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
        model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        dirtTexture.UseTexture();
        material.UseMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[0]->RenderMesh();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-10.0f, 0.0f, -20.0f));
  
       glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

       for (int i = 1; i < barNum; ++i)
       {
           model = glm::translate(model, glm::vec3( 2.0f, 0.0f, 0.0f));
           glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
           float yCoordinate = frequencies[i];
           float corr = 1.5 * yCoordinate * pow(2, i);
           glUniform1f(targetPos, corr);
           cubeTexture.UseTexture();
           material.UseMaterial(uniformSpecularIntensity, uniformShininess);
           meshList[i+1]->RenderMesh();
       }


        glUseProgram(0);
        mainWindow.SwapBuffers();
    }
}