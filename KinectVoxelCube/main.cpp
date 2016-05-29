#include "KinectScanner.h"
#include "directx/gk2_cubes.h"
#include "directx/gk2_window.h"
#include "directx/gk2_exceptions.h"
#include "FakeVoxelGridProvider.h"
#include "Reconstruction.h"

using namespace std;
using namespace gk2;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
    //KinectScannerFusion scanner;
    //scanner.Run();
    //auto scanData = scanner.GetScanData();
    
    //KinectScanData scanData;
    //int cubeSize = 20;
    //scanData.width = cubeSize;
    //scanData.height = cubeSize;
    //scanData.length = cubeSize;
    //scanData.data = createVoxelizedSphereInGrid(cubeSize, cubeSize, cubeSize, 0.33f*cubeSize);

	KinectScanner scanner;
	//scanner.InitKinectSensor();
	//scanner.PersistScene("scene10.yml");

	auto frames = scanner.LoadSceneFromFile("scene10.yml");

	Reconstruction rec;
	VoxelData grid = rec.GetVoxelGrid(frames);

    UNREFERENCED_PARAMETER(prevInstance);
    UNREFERENCED_PARAMETER(cmdLine);
    shared_ptr<ApplicationBase> app;
    shared_ptr<Window> w;
    int exitCode = 0;

    try
    {
        app.reset(new Cubes(hInstance, grid));
        w.reset(new Window(hInstance, 800, 800, L"Output"));
        exitCode = app->Run(w.get(), cmdShow);
    }
    catch (Exception& e)
    {
        MessageBoxW(nullptr, e.getMessage().c_str(), L"B��d", MB_OK);
        exitCode = e.getExitCode();
    }
    catch (std::exception& e)
    {
        string s(e.what());
        MessageBoxW(nullptr, wstring(s.begin(), s.end()).c_str(), L"B��d!", MB_OK);
    }
    catch (...)
    {
        MessageBoxW(nullptr, L"Nieznany B��d", L"B��d", MB_OK);
        exitCode = -1;
    }

    w.reset();
    app.reset();
    return exitCode;
}
