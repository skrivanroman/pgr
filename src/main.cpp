#define GLM_FORCE_RADIANS
#include <iostream>
#include <array>
#include <fstream>
#include "pgr.h"
#include "ShaderProgram.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include "utils.hpp"
#include "Skybox.hpp"
#include "Lights.hpp"
#include "Ground.hpp"
#include "Bezier.hpp"
#include "hardCoded.hpp"
#include "BSpline.hpp"
#include "Sprite.hpp"
#include "Water.hpp"

namespace skrivrom
{
    // current size of the window in pixels
    uint32_t WIN_WIDTH = 512 * 2;
    uint32_t WIN_HEIGHT = 512 * 2;
    const char* WIN_TITLE = "PGR";
    const float MAX_FREE_DISTANCE = 1000.0f;
    const int32_t DAY_DURATION = 100000;

    ShaderProgram program, skyboxProgram, simpleProgram, spriteProgram, waterProgram,
        barProgram;

    Model bin, bike, lamp, arcade, flashlight, rock, rock2, rock3, house, maple, tree, tree2,
        glass, glass2;
    Skybox skybox, skybox2;

    DirLight dirLight;
    PointLight pointLight;
    SpotLight spotLight;

    Ground ground;
    Water water;

    bool fogOn = false;
    bool clickPending = false;
    bool bikeMove = true;
    bool flashlightOn = false;
    bool mouseLook = true;

    Bezier bezier(
        std::vector<Bezier::Segment>{
            {
                glm::vec3(-300.0f, 250.0f, 0.0f),
                    glm::vec3(-300.0f, 250.0f, 166.0f),
                    glm::vec3(-166.0f, 250.0f, 300.0f),
                    glm::vec3(0.0f, 250.0f, 300.0f)
            },
    {
        glm::vec3(0.0f, 250.0f, 300.0f),
        glm::vec3(166.0f, 250.0f, 300.0f),
        glm::vec3(300.0f, 250.0f, 166.0f),
        glm::vec3(300.0f, 250.0f, 0.0f)
    },
        {
        glm::vec3(300.0f, 250.0f, 0.0f),
        glm::vec3(300.0f, 250.0f,-166.0f),
        glm::vec3(166.0f, 250.0f,-300.0f),
        glm::vec3(0.0f, 250.0f,-300.0f)
        },
    {
        glm::vec3(0.0f, 250.0f,-300.0f),
        glm::vec3(-166.0f,250.0f,-300.0f),
        glm::vec3(-300.0f,250.0f,-166.0f),
        glm::vec3(-300.0f,250.0f, 0.0f)
    }
    }
    );

    BSpline spline(std::vector<glm::vec3> {
        glm::vec3(350.0f, 120.0f, 0.0f),
            glm::vec3(250.0f, 160.0f, 250.0f),
            glm::vec3(0.0f, 140.0f, 350.0f),
            glm::vec3(-250.0f, 160.0f, 250.0f),
            glm::vec3(-350.0f, 120.0f, 0.0f),
            glm::vec3(-250.0f, 90.0f, -250.0f),
            glm::vec3(0.0f, 110.0f, -350.0f),
            glm::vec3(250.0f, 90.0f, -250.0f)
    });

    float splineCtr = 0.0f;
    float bezierCtr = 0.0f;

    Camera freeCamera({ -50.0f, 60.0f, 80.0f }, { 0.0592903f , -0.238533f , 0.969323f }, 0.3f, 60.0f, (float)WIN_WIDTH, (float)WIN_HEIGHT);
    Camera* camera = &freeCamera;
    Camera statCamera1({ 300.0f, 100.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, 0.0f, 60.0f, (float)WIN_WIDTH, (float)WIN_HEIGHT);
    Camera statCamera2({ 0.0f, 200.0f, 200.0f }, { 0.0f, 0.0f, -1.0f }, 0.0f, 60.0f, (float)WIN_WIDTH, (float)WIN_HEIGHT);
    Camera dynCamera1({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, 0.0f, 60.0f, (float)WIN_WIDTH, (float)WIN_HEIGHT);
    Camera dynCamera2({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, 0.0f, 60.0f, (float)WIN_WIDTH, (float)WIN_HEIGHT);
    bool freeView = true;

    AnimatedSprite sprite;
    GLuint fogTexture;

    // key table
    bool keyTable[256] = { false };
    bool specialKeyTable[512] = { false };

    std::vector<Model*> transparent;

    bool isColliding(const glm::vec3& camPos, const Model& model)
    {
        glm::mat4 modelMatrix = model.transform.calcModel();
        glm::mat4 inverseModelMatrix = glm::inverse(modelMatrix);

        glm::vec4 camLocalPos4 = inverseModelMatrix * glm::vec4(camera->postion, 1.0f);
        glm::vec3 camLocalPos = glm::vec3(camLocalPos4);

        glm::vec3 localBuffer = glm::vec3(2.0f) / model.transform.scale;

        glm::vec3 boxMin = model.modelBounds.minPoint - localBuffer;
        glm::vec3 boxMax = model.modelBounds.maxPoint + localBuffer;

        bool overlapX = camLocalPos.x >= boxMin.x && camLocalPos.x <= boxMax.x;
        bool overlapY = camLocalPos.y >= boxMin.y && camLocalPos.y <= boxMax.y;
        bool overlapZ = camLocalPos.z >= boxMin.z && camLocalPos.z <= boxMax.z;

        return overlapX && overlapY && overlapZ;
    }

    bool checkAllCollisions(const glm::vec3& positionToCheck)
    {
        if (isColliding(positionToCheck, bin))
        {
            return true;
        }
        if (isColliding(positionToCheck, arcade))
        {
            return true;
        }
        if (isColliding(positionToCheck, bike))
        {
            return true;
        }
        if (isColliding(positionToCheck, lamp))
        {
            return true;
        }
        if (isColliding(positionToCheck, rock2))
        {
            return true;
        }
        if (isColliding(positionToCheck, rock3))
        {
            return true;
        }

        return false;
    }

    void processInput();

    void update(int value)
    {
        processInput();

        if (bikeMove)
        {
            bezierCtr += 0.005f;
        }
        if (bezierCtr > bezier.segments.size())
        {
            bezierCtr = 0.0f;
        }

        size_t i = std::floor(bezierCtr);
        bezier.segments[i].moveObject(bike.transform, bezierCtr - i);

        dynCamera1.front = bezier.segments[i].getTangent(bezierCtr - i);
        dynCamera1.right = -glm::normalize(glm::cross(dynCamera1.worldUp, dynCamera1.front));
        dynCamera1.up = glm::normalize(glm::cross(dynCamera1.right, dynCamera1.front));

        dynCamera1.postion = bike.transform.position - dynCamera1.front * 100.0f + glm::vec3(0.0f, 30.0f, 0.0f);

        splineCtr += 0.008f;
        spline.moveCamera(dynCamera2, splineCtr);

        sprite.update(8.0f / 1000.0f);

        glutPostRedisplay();

        glutTimerFunc(16, update, 0);
    }

    void reshape(int width, int height)
    {
        if (height <= 0)
        {
            return;
        }

        glViewport(0, 0, width, height);

        camera->handleResize(width, height);

        WIN_WIDTH = width;
        WIN_HEIGHT = height;
    }

    void setupLights()
    {
        dirLight.direction = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.3f));
        dirLight.ambient = glm::vec3(0.25f, 0.25f, 0.30f);
        dirLight.diffuse = glm::vec3(0.75f, 0.75f, 0.70f);
        dirLight.specular = glm::vec3(0.5f, 0.5f, 0.5f);

        float centerX = (lamp.modelBounds.minPoint.x + lamp.modelBounds.maxPoint.x) * 0.5f;
        float centerZ = (lamp.modelBounds.minPoint.z + lamp.modelBounds.maxPoint.z) * 0.5f;
        float targetY = lamp.modelBounds.maxPoint.y - (lamp.modelBounds.height * 0.3f);
        glm::vec4 localPos = glm::vec4(centerX, targetY, centerZ, 1.0f);

        glm::mat4 lampMatrix = lamp.transform.calcModel();
        glm::vec4 worldPos = lampMatrix * localPos;

        pointLight.position = glm::vec3(worldPos);
        pointLight.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
        pointLight.diffuse = glm::vec3(0.9f, 0.7f, 0.5f);
        pointLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
        pointLight.constant = 1.0f;
        pointLight.linear = 0.09f;

        spotLight.position = camera->postion;
        spotLight.direction = camera->front;
        spotLight.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
        spotLight.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        spotLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
        spotLight.cutOff = cos(glm::radians(12.5f));
        spotLight.outerCutOff = cos(glm::radians(17.5f));
        spotLight.constant = 1.0f;
        spotLight.linear = 0.09f;
    }

    void init()
    {
        glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glEnable(GL_FRAMEBUFFER_SRGB);

        program = ShaderProgram(getShaderDir() + "vertex.glsl", getShaderDir() + "fragment.glsl");
        skyboxProgram = ShaderProgram(getShaderDir() + "skyboxVert.glsl", getShaderDir() + "skyboxFrag.glsl");
        simpleProgram = ShaderProgram(getShaderDir() + "simpleVert.glsl", getShaderDir() + "simpleFrag.glsl");
        spriteProgram = ShaderProgram(getShaderDir() + "spriteVert.glsl", getShaderDir() + "spriteFrag.glsl");
        waterProgram = ShaderProgram(getShaderDir() + "waterVert.glsl", getShaderDir() + "waterFrag.glsl");
        barProgram = ShaderProgram(getShaderDir() + "barVert.glsl", getShaderDir() + "barFrag.glsl");
        //groundProgram = ShaderProgram(getShaderDir() + "groundVert.glsl", getShaderDir() + "groundFrag.glsl");

        Material houseMat;
        houseMat.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        houseMat.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
        houseMat.specular = glm::vec3(0.1f, 0.1f, 0.1f);
        houseMat.shininess = 16.0f;
        house = Model(getAssetDir() + "house\\scene.gltf", program.location, houseMat);
        house.transform.position = glm::vec3(0.0f, 5.0f, -200.0f);
        house.transform.scale *= 50.0f;

        lamp = Model(getAssetDir() + "lamp\\scene.gltf", program.location);
        lamp.transform.position = glm::vec3(-75.0f, 7.0f, 20.0f);
        lamp.transform.scale *= 5.0f;

        bin = Model(getAssetDir() + "bin\\trashbin.obj", program.location);
        bin.transform.position = glm::vec3(35.0f, 2.0f, 50.0f);
        bin.transform.scale *= 0.5f;

        arcade = Model(getAssetDir() + "arcade\\scene.gltf", program.location);
        arcade.transform.position = glm::vec3(160.0f, 10.0f, -90.0f);
        arcade.transform.scale *= 20.0f;

        sprite = AnimatedSprite(getAssetDir() + "sprites\\idle.png", getAssetDir() + "sprites\\run.png", 5, 5, 10.0f);
        sprite.transform.scale *= 45.0f;
        sprite.transform.position = glm::vec3(160.0f, 95.5f, -90.5f);
        sprite.transform.rotation = glm::angleAxis(glm::radians(-15.0f), glm::vec3(1, 0, 0));

        Material bikeMat;
        bikeMat.diffuse = glm::vec3(0.45f, 0.48f, 0.52f);
        bikeMat.ambient = glm::vec3(0.08f, 0.08f, 0.10f);
        bikeMat.specular = glm::vec3(1.0f, 1.0f, 1.0f);
        bikeMat.shininess = 128.0f;

        bike = Model(getAssetDir() + "hover_bike\\scene.gltf", program.location, bikeMat);
        bike.transform.position = glm::vec3(30.0f, 2.0f, 30.0f);
        bike.transform.scale *= 0.2f;

        flashlight = Model(getAssetDir() + "flashlight\\scene.gltf", program.location);
        flashlight.transform.position = glm::vec3(15.0f, 6.0f, 40.0f);
        flashlight.transform.scale *= 20.0f;

        Material rockMat;
        rockMat.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        rockMat.ambient = glm::vec3(0.15f, 0.15f, 0.15f);
        rockMat.specular = glm::vec3(0.05f, 0.05f, 0.05f);
        rockMat.shininess = 8.0f;

        //rock = Model(getAssetDir() + "rock\\scene.obj", program.location);
        //rock.transform.position = glm::vec3(30.0f, 1.0f, 220.0f);
        //rock.transform.scale *= 5.0f;

        rock2 = Model(getAssetDir() + "rock2\\scene.gltf", program.location, rockMat);
        rock2.transform.position = glm::vec3(30.0f, 10.0f, 220.0f);
        rock2.transform.scale *= 60.0f;

        rock3 = Model(getAssetDir() + "rock3\\scene.gltf", program.location, rockMat);
        rock3.transform.position = glm::vec3(-200.0f, 20.0f, 220.0f);
        rock3.transform.scale *= 2000.0f;

        Material treeMat;
        treeMat.diffuse = glm::vec3(0.5f, 0.6f, 0.3f);
        treeMat.ambient = glm::vec3(0.15f, 0.15f, 0.1f);
        treeMat.specular = glm::vec3(0.0f, 0.0f, 0.0f);
        treeMat.shininess = 1.0f;

        tree = Model(getAssetDir() + "tree\\scene.gltf", program.location, treeMat);
        tree.transform.position = glm::vec3(-150.0f, 5.0f, 400.0f);
        tree.transform.scale *= 13.0f;

        tree2 = Model(getAssetDir() + "tree2\\scene.gltf", program.location, treeMat);
        tree2.transform.position = glm::vec3(50.0f, 5.0f, 420.0f);
        tree2.transform.scale *= 15.0f;

        maple = Model(getAssetDir() + "maple\\scene.gltf", program.location, treeMat);
        maple.transform.position = glm::vec3(180.0f, -5.0f, 300.0f);
        maple.transform.scale *= 1.0f;

        Material glassMat;
        glassMat.diffuse = glm::vec3(0.9f, 0.95f, 1.0f);
        glassMat.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
        glassMat.specular = glm::vec3(1.0f, 1.0f, 1.0f);
        glassMat.shininess = 128.0f;
        glassMat.alpha = 0.3f;

        glass = Model(getAssetDir() + "glass\\scene.gltf", program.location, glassMat);
        glass.transform.position = glm::vec3(50.0f, 3.0f, 20.0f);
        glass.transform.scale *= 100.0f;

        glass2 = Model(getAssetDir() + "glass2\\scene.gltf", program.location, glassMat);
        glass2.transform.position = glm::vec3(10.0f, 5.0f, 20.0f);
        glass2.transform.scale *= 5.0f;

        transparent.push_back(&tree);
        transparent.push_back(&tree2);
        transparent.push_back(&maple);
        transparent.push_back(&glass);
        transparent.push_back(&glass2);

        setupSimpleMesh();

        fogTexture = pgr::createTexture(getAssetDir() + "fog\\smoke4.jpg");

        water = Water(glm::vec3(500.0f, 3.0f, -100.0f), 120.0f, getAssetDir() + "water\\pool.jpg");

        std::vector<std::string> faces = {
            "rt",
            "lf",
            "up",
            "dn",
            "bk",
            "ft",
        };
        skybox = Skybox(getAssetDir() + "skybox\\s1_", faces);
        skybox2 = Skybox(getAssetDir() + "skybox\\s2_", faces);

        ground = Ground(
            10000.0f,
            1,
            40,
            program.location,
            getAssetDir() + "ground\\textures\\forest_ground_04_diff_4k.jpg",
            getAssetDir() + "rocks\\textures\\coast_sand_rocks_02_diff_4k.jpg"
        );

        setupLights();
        glutTimerFunc(0, update, 0);
    }

    // draw hardcoded mesh
    void drawSimpleMesh()
    {
        simpleProgram.use();
        glm::mat4 PV = camera->calculatePV();
        glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 10.0f);
        model = glm::translate(model, glm::vec3(50.0f, 0.0f, 50.0f));
        glUniformMatrix4fv(simpleProgram.location.PV, 1, GL_FALSE, glm::value_ptr(PV));
        glUniformMatrix4fv(simpleProgram.location.model, 1, GL_FALSE, glm::value_ptr(model));

        drawHardCoded();
    }

    void drawSkybox(const Skybox& currentSkybox)
    {
        skyboxProgram.use();
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);

        glm::mat4 PV = camera->getSkyboxPV();
        glUniformMatrix4fv(skyboxProgram.location.PV, 1, GL_FALSE, glm::value_ptr(PV));

        currentSkybox.draw();

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

    // used for ui
    void drawFullScreenQuad(const ShaderProgram::Location& location)
    {
        static GLuint quadVAO = 0;
        static GLuint quadVBO = 0;

        if (quadVAO == 0)
        {
            float quadVertices[] = {
                -1.0f,  1.0f,  0.0f, 1.0f,
                -1.0f, -1.0f,  0.0f, 0.0f,
                 1.0f, -1.0f,  1.0f, 0.0f,

                -1.0f,  1.0f,  0.0f, 1.0f,
                 1.0f, -1.0f,  1.0f, 0.0f,
                 1.0f,  1.0f,  1.0f, 1.0f
            };

            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);

            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(location.postion);
            glVertexAttribPointer(location.postion, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

            glEnableVertexAttribArray(location.texCoords);
            glVertexAttribPointer(location.texCoords, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void draw()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 PV = camera->calculatePV();

        bool isDay = ((glutGet(GLUT_ELAPSED_TIME) % DAY_DURATION) < (DAY_DURATION / 2));
        if (isDay)
        {
            drawSkybox(skybox);
        }
        else
        {
            drawSkybox(skybox2);
        }

        drawSimpleMesh();

        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
        waterProgram.use();
        glUniformMatrix4fv(waterProgram.location.PV, 1, GL_FALSE, glm::value_ptr(PV));
        glUniform1i(waterProgram.location.sampler, 0);
        water.draw(waterProgram.location, currentTime);

        glStencilFunc(GL_ALWAYS, 6, 0xFF);
        spriteProgram.use();
        glUniformMatrix4fv(spriteProgram.location.PV, 1, GL_FALSE, glm::value_ptr(PV));
        sprite.draw(spriteProgram.location);

        program.use();

        glUniformMatrix4fv(program.location.PV, 1, GL_FALSE, glm::value_ptr(PV));
        glUniform3fv(program.location.viewPos, 1, glm::value_ptr(camera->postion));
        glUniform1i(program.location.sampler, 0);
        glUniform1i(program.location.hasTwoTextures, false);
        glUniform1i(program.location.fogOn, fogOn);
        glUniform1i(program.location.flashlightOn, flashlightOn);
        CHECK_GL_ERROR();

        spotLight.position = camera->postion;
        spotLight.direction = camera->front;

        if (isDay)
        {
            dirLight.ambient = glm::vec3(0.25f, 0.25f, 0.30f);
            dirLight.diffuse = glm::vec3(0.75f, 0.75f, 0.70f);
            dirLight.specular = glm::vec3(0.5f, 0.5f, 0.5f);
        }
        else
        {
            dirLight.ambient = glm::vec3(0.01f, 0.015f, 0.015f);
            dirLight.diffuse = glm::vec3(0.01f, 0.01f, 0.01f);
            dirLight.specular = glm::vec3(0.f, 0.f, 0.f);
        }

        dirLight.apply(program.location);
        pointLight.apply(program.location);
        spotLight.apply(program.location);


        glUniform2f(program.location.resolution, WIN_WIDTH, WIN_HEIGHT);

        glUniform1f(program.location.time, currentTime);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, fogTexture);
        glUniform1i(program.location.fogTexture, 2);


        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        bin.draw(program.location);

        glStencilFunc(GL_ALWAYS, 2, 0xFF);
        arcade.draw(program.location);

        glStencilFunc(GL_ALWAYS, 3, 0xFF);
        bike.draw(program.location);

        glStencilFunc(GL_ALWAYS, 4, 0xFF);
        lamp.draw(program.location);

        glStencilFunc(GL_ALWAYS, 7, 0xFF);
        flashlight.draw(program.location);

        glStencilFunc(GL_ALWAYS, 5, 0xFF);
        glUniform1i(program.location.hasTwoTextures, true);
        ground.draw(program.location);
        glUniform1i(program.location.hasTwoTextures, false);

        house.draw(program.location);

        //tree.draw(program.location);
        //tree2.draw(program.location);
        //maple.draw(program.location);
        //rock.draw(program.location);
        rock2.draw(program.location);
        rock3.draw(program.location);


        // draw transparent objects
        glDisable(GL_STENCIL_TEST);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        std::sort(transparent.begin(), transparent.end(),
            [&](Model* first, Model* second)
            {
                float distFirst = glm::length(camera->postion - first->transform.position);
                float distSecond = glm::length(camera->postion - second->transform.position);

                return distFirst > distSecond;
            });

        for (Model* m : transparent)
        {
            m->draw(program.location);
        }

        glDepthMask(GL_TRUE);


        glDisable(GL_DEPTH_TEST);

        barProgram.use();
        float currentTimeMilli = static_cast<float>(glutGet(GLUT_ELAPSED_TIME));
        glUniform1f(barProgram.location.time, currentTimeMilli);

        drawFullScreenQuad(barProgram.location);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        // object interaction
        if (clickPending)
        {
            GLubyte pickedID = 0;
            glReadPixels(WIN_WIDTH / 2, WIN_HEIGHT / 2, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &pickedID);

            switch (pickedID)
            {
            case 1: std::cout << "Clicked: Trash Bin" << '\n'; break;
            case 2: std::cout << "Clicked: Arcade" << '\n'; break;
            case 3:
                std::cout << "Clicked: Hover Bike" << '\n';
                bikeMove = !bikeMove;
                break;
            case 4: std::cout << "Clicked: Lamp" << '\n'; break;
            case 5: std::cout << "Clicked: Ground" << '\n'; break;
            case 6:
                std::cout << "Clicked Character Sprite!" << std::endl;
                sprite.toggleState();
                break;
            case 7: std::cout << "Clicked: flashlight" << '\n';
                flashlight.visible = false;
                flashlightOn = true;
                break;
            default: std::cout << "Clicked: Nothing" << std::endl; break;
            }

            clickPending = false;
        }
        CHECK_GL_ERROR();
        glutSwapBuffers();
    }

    void onMouse(int x, int y)
    {
        if (freeView)
        {
            camera->processMouse(x, y);
        }
    }

    void loadConfig(const std::string& filename, std::vector<Transform*>& transforms)
    {
        std::ifstream file(filename);

        if (!file.is_open())
        {
            std::cerr << "Failed to open file: " << filename << '\n';
            return;
        }

        float x = 0.0f, y = 0.0f, z = 0.0f;
        size_t index = 0;

        while (file >> x >> y >> z)
        {
            if (index >= transforms.size())
            {
                break;
            }

            if (transforms[index] != nullptr)
            {
                transforms[index]->position = glm::vec3(x, y, z);
            }

            index++;
        }

        file.close();
    }

    void saveConfig(const std::string& filename, const std::vector<Transform*>& transforms)
    {
        std::ofstream file(filename);

        if (!file.is_open())
        {
            std::cerr << "Failed to open file: " << filename << '\n';
            return;
        }

        for (size_t i = 0; i < transforms.size(); ++i)
        {
            if (transforms[i] != nullptr)
            {
                file << transforms[i]->position.x << " "
                    << transforms[i]->position.y << " "
                    << transforms[i]->position.z << "\n";
            }
        }

        file.close();
    }

    void ExitProgram()
    {
        std::vector<Transform*> trans;
        trans.push_back(&bin.transform);
        trans.push_back(&lamp.transform);
        trans.push_back(&house.transform);
        trans.push_back(&arcade.transform);
        trans.push_back(&rock2.transform);
        saveConfig(getAssetDir() + "config.txt", trans);
        exit(0);
    }

    //handels all keyboard input
    void processInput()
    {
        if (specialKeyTable[GLUT_KEY_F1])
        {
            camera = &freeCamera;
            freeView = true;
            specialKeyTable[GLUT_KEY_F1] = false;
        }
        if (specialKeyTable[GLUT_KEY_F2])
        {
            camera = &statCamera1;
            freeView = false;
            specialKeyTable[GLUT_KEY_F2] = false;
        }
        if (specialKeyTable[GLUT_KEY_F3])
        {
            camera = &statCamera2;
            freeView = false;
            specialKeyTable[GLUT_KEY_F3] = false;
        }
        if (specialKeyTable[GLUT_KEY_F4])
        {
            camera = &dynCamera1;
            freeView = false;
            specialKeyTable[GLUT_KEY_F4] = false;
        }
        if (specialKeyTable[GLUT_KEY_F5])
        {
            camera = &dynCamera2;
            freeView = false;
            specialKeyTable[GLUT_KEY_F5] = false;
        }

        const int ESCAPE = 27;
        if (keyTable['q'] || keyTable[ESCAPE])
        {
            ExitProgram();
        }

        if (keyTable['g'])
        {
            fogOn = !fogOn;
            keyTable['g'] = false;
        }

        if (keyTable['v'])
        {
            std::vector<Transform*> trans;
            trans.push_back(&bin.transform);
            trans.push_back(&lamp.transform);
            trans.push_back(&house.transform);
            trans.push_back(&arcade.transform);
            trans.push_back(&rock2.transform);
            loadConfig(getAssetDir() + "\\config.txt", trans);
        }

        const int CTRL_E = 5;
        if (keyTable[CTRL_E])
        {
            std::cout << "Bike is at: ( " << bike.transform.position.x << ", " <<
                bike.transform.position.y << ", " << bike.transform.position.z << " )\n";
            keyTable[CTRL_E] = false;
        }

        if (freeView)
        {
            glm::vec3 oldPos = freeCamera.postion;
            bool moved = false;

            if (keyTable['w'])
            {
                freeCamera.processKeyboard('w');
                moved = true;
            }
            if (keyTable['s'])
            {
                freeCamera.processKeyboard('s');
                moved = true;
            }
            if (keyTable['a'])
            {
                freeCamera.processKeyboard('a');
                moved = true;
            }
            if (keyTable['d'])
            {
                freeCamera.processKeyboard('d');
                moved = true;
            }
            if (keyTable['x'])
            {
                freeCamera.processKeyboard('x');
                moved = true;
            }
            if (keyTable['z'])
            {
                freeCamera.processKeyboard('z');
                moved = true;
            }

            if (moved)
            {
                float distance = glm::length(freeCamera.postion);
                if (checkAllCollisions(freeCamera.postion) || freeCamera.postion.y < 0 || distance >= MAX_FREE_DISTANCE)
                {
                    freeCamera.postion = oldPos;
                }
            }
        }
    }

    void onKeyboard(unsigned char key, int x, int y)
    {
        keyTable[key] = true;
    }
    void onSpecialKeys(int key, int x, int y)
    {
        if (key >= 0 && key < 512)
        {
            specialKeyTable[key] = true;
        }
    }
    void onMouseClick(int button, int state, int x, int y)
    {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            if (freeView)
            {
                clickPending = true;
            }
        }

        if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
        {
            if (mouseLook)
            {
                glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
            }
            else
            {
                glutSetCursor(GLUT_CURSOR_NONE);
            }

            mouseLook = !mouseLook;
            freeView = !freeView;
        }
    }

    void onKeyboardUp(unsigned char key, int x, int y)
    {
        keyTable[key] = false;
    }

    void onSpecialKeysUp(int key, int x, int y)
    {
        if (key >= 0 && key < 512)
        {
            specialKeyTable[key] = false;
        }
    }

}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL | GLUT_MULTISAMPLE);
	glutInitWindowSize(skrivrom::WIN_WIDTH, skrivrom::WIN_HEIGHT);
	glutCreateWindow(skrivrom::WIN_TITLE);

	glutDisplayFunc(skrivrom::draw);
	glutKeyboardFunc(skrivrom::onKeyboard);
	glutSpecialFunc(skrivrom::onSpecialKeys);
	glutKeyboardUpFunc(skrivrom::onKeyboardUp);
	glutSpecialUpFunc(skrivrom::onSpecialKeysUp);
	glutMotionFunc(skrivrom::onMouse);
	glutPassiveMotionFunc(skrivrom::onMouse);
	glutMouseFunc(skrivrom::onMouseClick);

	glutReshapeFunc(skrivrom::reshape);
	glutSetCursor(GLUT_CURSOR_NONE);
	glutWarpPointer(skrivrom::WIN_WIDTH / 2, skrivrom::WIN_HEIGHT / 2);

	if (!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR))
		pgr::dieWithError("pgr init failed, required OpenGL not supported?");

	skrivrom::init();

	std::cout << "Starting render loop!" << std::endl;

	glutMainLoop();
	return 0;
}
