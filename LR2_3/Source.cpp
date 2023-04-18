#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
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
#include "Magick++.h"
#include "billboard_list.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 1024


class Main : public ICallbacks
{
public:

    Main()
    {
        m_pLightingTechnique = NULL;
        m_pGameCamera = NULL;
        m_pGround = NULL;
        m_pTexture = NULL;
        m_pNormalMap = NULL;

        m_dirLight.AmbientIntensity = 0.2f;
        m_dirLight.DiffuseIntensity = 0.8f;
        m_dirLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
        m_dirLight.Direction = Vector3f(1.0f, 0.0f, 0.0f);

        m_persProjInfo.FOV = 60.0f;
        m_persProjInfo.Height = WINDOW_HEIGHT;
        m_persProjInfo.Width = WINDOW_WIDTH;
        m_persProjInfo.zNear = 1.0f;
        m_persProjInfo.zFar = 100.0f;

    }

    ~Main()
    {
        SAFE_DELETE(m_pLightingTechnique);
        SAFE_DELETE(m_pGameCamera);
        SAFE_DELETE(m_pGround);
        SAFE_DELETE(m_pTexture);
        SAFE_DELETE(m_pNormalMap);
    }

    bool Init()
    {
        // параметры камеры
        Vector3f Pos(0.5f, 2.0f, 0.25f);
        Vector3f Target(0.0f, -0.5f, 1.0f);
        Vector3f Up(0.0, 1.0f, 0.0f);

        m_pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);

        // инициализируем эффект освещения
        m_pLightingTechnique = new LightingTechnique();

        if (!m_pLightingTechnique->Init()) {
            printf("Error initializing the lighting technique\n");
            return false;
        }

        m_pLightingTechnique->Enable();
        m_pLightingTechnique->SetDirectionalLight(m_dirLight);
        m_pLightingTechnique->SetColorTextureUnit(0);
        m_pLightingTechnique->SetNormalMapTextureUnit(2);

        // создаем меш
        m_pGround = new Mesh();

        if (!m_pGround->LoadMesh("C:/Users/user/Desktop/Учеба/С++ программы/ИКГ/Open_GL_LR5/LR_3/LR2_3/quad.obj")) {
            return false;
        }

        if (!m_billboardList.Init("C:/Users/user/Desktop/Учеба/С++ программы/ИКГ/Open_GL_LR5/LR_3/LR2_3/rose.png")) {
            return false;
        }

         //загружем текстуру
        m_pTexture = new Texture(GL_TEXTURE_2D, "C:/Users/user/Desktop/Учеба/С++ программы/ИКГ/Open_GL_LR5/LR_3/LR2_3/grass.jpg");

        if (!m_pTexture->Load()) {
            return false;
        }

         //привязываем текстуру к m_pTexture
        m_pTexture->Bind(COLOR_TEXTURE_UNIT);

        // загружаем карту нормалей
        m_pNormalMap = new Texture(GL_TEXTURE_2D, "C:/Users/user/Desktop/Учеба/С++ программы/ИКГ/Open_GL_LR5/LR_3/LR2_3/normal_map.jpg");

        if (!m_pNormalMap->Load()) {
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
        m_pGameCamera->OnRender();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_pLightingTechnique->Enable();

        // пайплайн
        Pipeline p;
        p.Scale(20.0f, 20.0f, 1.0f);
        p.Rotate(90.0f, 0.0, 0.0f);
        // устанавливаем камеру
        p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
        p.SetPerspectiveProj(m_persProjInfo);

        m_pTexture->Bind(COLOR_TEXTURE_UNIT);

        m_pLightingTechnique->SetWVP(p.GetWVPTrans());
        m_pLightingTechnique->SetWorldMatrix(p.GetWorldTrans());
        m_pGround->Render();

        m_billboardList.Render(p.GetVPTrans(), m_pGameCamera->GetPos());
        // меняем фоновый буфер и буфер кадра местами
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

private:

    LightingTechnique* m_pLightingTechnique;
    Camera* m_pGameCamera;
    DirectionalLight m_dirLight;
    Mesh* m_pGround;
    Texture* m_pTexture;
    Texture* m_pNormalMap;
    PersProjInfo m_persProjInfo;
    BillboardList m_billboardList;
};

int main(int argc, char** argv)
{
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
