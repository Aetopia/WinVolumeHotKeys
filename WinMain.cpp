#include <initguid.h>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
    MSG msg = {0};
    IMMDeviceEnumerator *pIMMDeviceEnumerator = NULL;
    IMMDevice *pIMMDevice = NULL;
    IAudioEndpointVolume *pIAudioEndpointVolumeRender = NULL,
                         *pIAudioEndpointVolumeCapture = NULL;

    CoInitialize(NULL);
    RegisterHotKey(0, 1, MOD_WIN | MOD_CONTROL, VK_OEM_PLUS);
    RegisterHotKey(0, 2, MOD_WIN | MOD_CONTROL, VK_OEM_MINUS);
    RegisterHotKey(0, 3, MOD_WIN | MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, 0x4D);
    RegisterHotKey(0, 4, MOD_WIN | MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT, 0x4D);

    CoCreateInstance(__uuidof(MMDeviceEnumerator),
                     NULL,
                     CLSCTX_INPROC_SERVER,
                     __uuidof(IMMDeviceEnumerator),
                     (LPVOID *)&pIMMDeviceEnumerator);

    pIMMDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pIMMDevice);
    pIMMDevice->Activate(__uuidof(IAudioEndpointVolume),
                         CLSCTX_ALL,
                         NULL,
                         (LPVOID *)&pIAudioEndpointVolumeRender);
    pIMMDevice->Release();

    pIMMDeviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pIMMDevice);
    pIMMDevice->Activate(__uuidof(IAudioEndpointVolume),
                         CLSCTX_ALL,
                         NULL,
                         (LPVOID *)&pIAudioEndpointVolumeCapture);
    pIMMDevice->Release();

    pIMMDeviceEnumerator->Release();
    pIMMDeviceEnumerator = NULL;
    pIMMDevice = NULL;

    while (GetMessageW(&msg, NULL, 0, 0))
    {
        if (msg.message != WM_HOTKEY)
            continue;
        DWORD dwModifiers = LOWORD(msg.lParam),
              dwVk = HIWORD(msg.lParam);
        FLOAT fLevel = 0.0;
        BOOL bMute = FALSE;

        if (dwModifiers == (MOD_WIN | MOD_CONTROL) && dwVk == VK_OEM_PLUS)
        {
            pIAudioEndpointVolumeRender->GetMasterVolumeLevelScalar(&fLevel);
            pIAudioEndpointVolumeRender->SetMasterVolumeLevelScalar(fLevel + 0.01f, NULL);
        }
        else if (dwModifiers == (MOD_WIN | MOD_CONTROL) && dwVk == VK_OEM_MINUS)
        {
            pIAudioEndpointVolumeRender->GetMasterVolumeLevelScalar(&fLevel);
            pIAudioEndpointVolumeRender->SetMasterVolumeLevelScalar(fLevel - 0.01f, NULL);
        }
        else if (dwModifiers == (MOD_WIN | MOD_CONTROL | MOD_ALT) && dwVk == 0x4D)
        {
            pIAudioEndpointVolumeRender->GetMute(&bMute);
            pIAudioEndpointVolumeRender->SetMute(!bMute, NULL);
        }
        else if (dwModifiers == (MOD_WIN | MOD_CONTROL | MOD_SHIFT) && dwVk == 0x4D)
        {
            pIAudioEndpointVolumeCapture->GetMute(&bMute);
            pIAudioEndpointVolumeCapture->SetMute(!bMute, NULL);
        }
    }

    return 0;
}