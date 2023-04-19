#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "math_3d.h"
#include "pipeline.h"
#include "Camera.h"
#include "texture.h"
#include "lighting_technique.h"
#include "glut_backend.h"
#include "ogldev_util.h"
#include "mesh.h"


#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 1024

class Main : public ICallbacks
{
public:

    Main()
    {
        m_pGameCamera = NULL;
        m_pEffect = NULL;
        m_scale = 0.0f;
        m_directionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
        m_directionalLight.AmbientIntensity = 1.0f;
        m_directionalLight.DiffuseIntensity = 1.0f;
        m_directionalLight.Direction = Vector3f(1.0f, 0.0, 0.0);

        m_persProjInfo.FOV = 60.0f;
        m_persProjInfo.Height = WINDOW_HEIGHT;
        m_persProjInfo.Width = WINDOW_WIDTH;
        m_persProjInfo.zNear = 1.0f;
        m_persProjInfo.zFar = 100.0f;

        m_pMesh1 = NULL;
        m_pMesh2 = NULL;
        m_pMesh3 = NULL;

    }

    ~Main()
    {
        SAFE_DELETE(m_pEffect);
        SAFE_DELETE(m_pGameCamera);
        SAFE_DELETE(m_pMesh1);
        SAFE_DELETE(m_pMesh2);
        SAFE_DELETE(m_pMesh3);
    }

    bool Init()
    {
        // параметры камеры
        Vector3f Pos(3.0f, 7.0f, -10.0f);
        Vector3f Target(0.0f, -0.2f, 1.0f);
        Vector3f Up(0.0, 1.0f, 0.0f);

        m_pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);

        // инициализируем эффект освещения
        m_pEffect = new LightingTechnique();

        if (!m_pEffect->Init()) {
            printf("Error initializing the lighting technique\n");
            return false;
        }

        m_pEffect->Enable();
        m_pEffect->SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
        m_pEffect->SetDirectionalLight(m_directionalLight);
        m_pEffect->SetMatSpecularIntensity(100.0f);
        m_pEffect->SetMatSpecularPower(32);

        // создаем меш
        m_pMesh1 = new Mesh();

        if (!m_pMesh1->LoadMesh("C:/Users/user/Desktop/Учеба/С++ программы/ИКГ/Open_GL_LR5(32-33)/LR_3/LR2_3/phoenix_ugv.md2")) {
            return false;
        }

        m_pMesh2 = new Mesh();

        if (!m_pMesh2->LoadMesh("C:/Users/user/Desktop/Учеба/С++ программы/ИКГ/Open_GL_LR5(32-33)/LR_3/LR2_3/jeep.obj")) {
            return false;
        }

        m_pMesh3 = new Mesh();

        if (!m_pMesh3->LoadMesh("C:/Users/user/Desktop/Учеба/С++ программы/ИКГ/Open_GL_LR5(32-33)/LR_3/LR2_3/hheli.obj")) {
            return false;
        }

        return true;
    }


    void Run()
    {
        GLUTBackendRun(this);
    }

    virtual void RenderSceneCB()
    {
        m_scale += 0.01f;

        m_pGameCamera->OnRender();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_pEffect->SetEyeWorldPos(m_pGameCamera->GetPos());

        // пайплайн
        Pipeline p;
        p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
        p.Rotate(0.0f, m_scale, 0.0f);
        p.SetPerspectiveProj(m_persProjInfo);


        p.Scale(0.1f, 0.1f, 0.1f);
        p.WorldPos(-6.0f, -2.0f, 10.0f);
        m_pEffect->SetWVP(p.GetWVPTrans());
        m_pEffect->SetWorldMatrix(p.GetWorldTrans());
        m_pMesh1->Render();

        p.Scale(0.01f, 0.01f, 0.01f);
        p.WorldPos(6.0f, -2.0f, 10.0f);
        m_pEffect->SetWVP(p.GetWVPTrans());
        m_pEffect->SetWorldMatrix(p.GetWorldTrans());
        m_pMesh2->Render();

        p.Scale(0.04f, 0.04f, 0.04f);
        p.WorldPos(0.0f, 6.0f, 10.0f);
        m_pEffect->SetWVP(p.GetWVPTrans());
        m_pEffect->SetWorldMatrix(p.GetWorldTrans());
        m_pMesh3->Render();

        glutSwapBuffers();
    }

    virtual void IdleCB()
    {
        RenderSceneCB();
    }

    virtual void SpecialKeyboardCB(int Key, int x, int y)
    {
        m_pGameCamera->OnKeyboard(Key);
    }


    virtual void KeyboardCB(unsigned char Key, int x, int y)
    {
        // выбор клавиш
        switch (Key) {
        case 'q':
            glutLeaveMainLoop();
            break;
        }
    }

    virtual void PassiveMouseCB(int x, int y)
    {
        m_pGameCamera->OnMouse(x, y);
    }

    virtual void MouseCB(int Button, int State, int x, int y)
    {
    }

private:

    LightingTechnique* m_pEffect;
    Camera* m_pGameCamera;
    float m_scale;
    DirectionalLight m_directionalLight;
    Mesh* m_pMesh1;
    Mesh* m_pMesh2;
    Mesh* m_pMesh3;
    PersProjInfo m_persProjInfo;
};

int main(int argc, char** argv)
{
   //srand(_getpid());

    SRANDOM;

    // инициализация
    GLUTBackendInit(argc, argv);
    
    // инициализация Magick
    Magick::InitializeMagick(*argv);

    // создаем окно
    if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 144, true, "OpenGL")) {
        return 1;
    }

    Main* pApp = new Main();


    if (!pApp->Init()) {
        return 1;
    }

    // основной цикл
    pApp->Run();

    delete pApp;

    return 0;
}
