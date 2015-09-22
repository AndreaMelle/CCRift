#include "Win32_GLAppUtil.h"
#include <Kernel/OVR_System.h>
#include "OVR_CAPI_GL.h"

using namespace OVR;

static bool MainLoop(bool retryCreate)
{
    Scene         * roomScene = nullptr; 

	ovrGraphicsLuid luid;
    ovrSizei windowSize = { 1920 / 2, 1080 / 2 };
	float aspectRatio = 1920.0f / 1080.0f;
    if (!Platform.InitDevice(windowSize.w, windowSize.h, reinterpret_cast<LUID*>(&luid)))
        goto Done;

    // Turn off vsync to let the compositor do its magic
    wglSwapIntervalEXT(0);

    roomScene = new Scene(false);

    bool isVisible = true;

    // Main loop
    while (Platform.HandleMessages())
    {
        static float Yaw(3.141592f);  
        if (Platform.Key[VK_LEFT])  Yaw += 0.002f;
        if (Platform.Key[VK_RIGHT]) Yaw -= 0.002f;

        static Vector3f Pos2(0.0f,1.6f,-5.0f);
        if (Platform.Key['W']||Platform.Key[VK_UP])     Pos2+=Matrix4f::RotationY(Yaw).Transform(Vector3f(0,0,-0.005f));
        if (Platform.Key['S']||Platform.Key[VK_DOWN])   Pos2+=Matrix4f::RotationY(Yaw).Transform(Vector3f(0,0,+0.005f));
        if (Platform.Key['D'])                          Pos2+=Matrix4f::RotationY(Yaw).Transform(Vector3f(+0.005f,0,0));
        if (Platform.Key['A'])                          Pos2+=Matrix4f::RotationY(Yaw).Transform(Vector3f(-0.005f,0,0));


        // Get view and projection matrices
        Matrix4f rollPitchYaw = Matrix4f::RotationY(Yaw);
		Matrix4f finalRollPitchYaw = rollPitchYaw;// *Matrix4f(EyeRenderPose[eye].Orientation);
        Vector3f finalUp = finalRollPitchYaw.Transform(Vector3f(0, 1, 0));
        Vector3f finalForward = finalRollPitchYaw.Transform(Vector3f(0, 0, -1));
		Vector3f shiftedEyePos = Pos2;// +rollPitchYaw.Transform(EyeRenderPose[eye].Position);

        Matrix4f view = Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
		float verticalFovRadians = 60.0f * 3.14159265 / 180.0f;
		ovrFovPort fov;
		fov.DownTan = tan(verticalFovRadians / 2);
		fov.UpTan = fov.DownTan;
		fov.LeftTan = tan((aspectRatio * verticalFovRadians) / 2);
		fov.RightTan = fov.LeftTan;
        
		Matrix4f proj = ovrMatrix4f_Projection(fov, 0.2f, 1000.0f, ovrProjection_RightHanded);

		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        roomScene->Render(view, proj);

        SwapBuffers(Platform.hDC);
    }

Done:
    delete roomScene;
    Platform.ReleaseDevice();
	return true;
}

//-------------------------------------------------------------------------------------
//int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
//{
//    //OVR::System::Init();
//
//    // Initializes LibOVR, and the Rift
//    //ovrResult result = ovr_Initialize(nullptr);
//    //VALIDATE(OVR_SUCCESS(result), "Failed to initialize libOVR.");
//
//    VALIDATE(Platform.InitWindow(hinst, L"Oculus Room Tiny (GL)"), "Failed to open window.");
//
//    Platform.Run(MainLoop);
//
//    //ovr_Shutdown();
//
//    //OVR::System::Destroy();
//
//    return(0);
//}
