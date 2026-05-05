#define GLM_FORCE_RADIANS
#include <iostream>
#include "pgr.h"
#include "ShaderProgram.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include "utils.hpp"
#include "Skybox.hpp"
#include "Lights.hpp"

uint32_t WIN_WIDTH = 512*2;
uint32_t WIN_HEIGHT = 512*2;
const char* WIN_TITLE = "PGR";

ShaderProgram program, skyboxProgram;
Model gauc, bin;
Skybox skybox;

DirLight dirLight;
PointLight pointLight;
SpotLight spotLight;

int ctr = 0.0f;

Camera camera({ 112.0f, 116.0f, -257.0f }, {  0.0592903f , -0.238533f , 0.969323f }, 0.3f, 60.0f, (float)WIN_WIDTH, (float)WIN_HEIGHT);

void timer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void reshape(int width, int height)
{
    if (height <= 0)
    {
        return;
    }
    std::cout << "REshape\n" << width << " " << height << '\n';

    glViewport(0, 0, width, height);

    camera.handleResize(width, height);

    WIN_WIDTH = width;
    WIN_HEIGHT = height;
}

void setupLights()
{
    dirLight.direction = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.3f));
    dirLight.ambient = glm::vec3(0.25f, 0.25f, 0.30f);
    dirLight.diffuse = glm::vec3(0.75f, 0.75f, 0.70f);
    dirLight.specular = glm::vec3(0.5f, 0.5f, 0.5f);

    pointLight.position = glm::vec3(2.0f, 200.0f, 2.0f);
    pointLight.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    pointLight.diffuse = glm::vec3(0.9f, 0.7f, 0.5f);
    pointLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    pointLight.constant = 1.0f;
    pointLight.linear = 0.09f;

    spotLight.position = camera.postion;
    spotLight.direction = camera.front;
    spotLight.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
    spotLight.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    spotLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    spotLight.cutOff = cos(glm::radians(12.5f));
    spotLight.outerCutOff = cos(glm::radians(17.5f));
    spotLight.constant = 1.0f;
    spotLight.linear = 0.09f;
}

void init() {
    glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
    //glEnable(GL_FRAMEBUFFER_SRGB);

    program = ShaderProgram(getShaderDir() + "vertex.glsl", getShaderDir() + "newFrag.glsl");
    skyboxProgram = ShaderProgram(getShaderDir() + "skyboxVert.glsl", getShaderDir() + "skyboxFrag.glsl");
    gauc = Model(getAssetDir() + "gauc\\gauc.obj", program.location);
    bin = Model(getAssetDir() + "bin\\trashbin.obj", program.location);
    bin.transform.position.x = -70.0f;

    std::vector<std::string> faces = {
	    "rt",
		"lf",
		"up",
		"dn",
		"bk",
		"ft",
    };
    skybox = Skybox(getAssetDir() + "skybox\\s1_", faces);

    setupLights();
    glutTimerFunc(0, timer, 0);
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program.use();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 PV = camera.calculatePV();

    bool info = false;
    ctr++;
    if (ctr > 100.0f && info)
    {
        ctr = 0.0f;
        std::cout << "Cam x: " << camera.postion.x << " y: " << camera.postion.y << " z: " << camera.postion.z << '\n';
        std::cout << "Cam dir x: " << camera.front.x << " y: " << camera.front.y << " z: " << camera.front.z << '\n';
    }

    glUniformMatrix4fv(program.location.PV, 1, GL_FALSE, glm::value_ptr(PV));
    glUniform3fv(program.location.viewPos, 1, glm::value_ptr(camera.postion));
    glUniform1i(program.location.sampler, 0);

    spotLight.position = camera.postion;
    spotLight.direction = camera.front;

    dirLight.apply(program.location);
    pointLight.apply(program.location);
    spotLight.apply(program.location);

    bin.draw(program.location);
    gauc.draw(program.location);

    skyboxProgram.use();
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    PV = camera.getSkyboxPV();
    glUniformMatrix4fv(skyboxProgram.location.PV, 1, GL_FALSE, glm::value_ptr(PV));

    skybox.draw();

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    CHECK_GL_ERROR();
    glutSwapBuffers();
}

void onMouse(int x, int y)
{
    camera.processMouse(x, y);
}

void onKeyboard(unsigned char key, int x, int y)
{
    camera.processKeyboard(key, x, y);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
    glutCreateWindow(WIN_TITLE);

    glutDisplayFunc(draw);
    glutKeyboardFunc(onKeyboard);
    glutMotionFunc(onMouse);
    glutPassiveMotionFunc(onMouse);

    glutSetCursor(GLUT_CURSOR_NONE);
	glutWarpPointer(WIN_WIDTH / 2, WIN_HEIGHT / 2);

    if (!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR))
        pgr::dieWithError("pgr init failed, required OpenGL not supported?");

    init();

    std::cout << "Starting render loop!" << std::endl;

    glutMainLoop();
    return 0;
}
