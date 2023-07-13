
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
    RegisterHotKey(0, 3, MOD_WIN | MOD_SHIFT | MOD_NOREPEAT, VK_F1);
    RegisterHotKey(0, 4, MOD_WIN | MOD_SHIFT | MOD_NOREPEAT, VK_F2);
    RegisterHotKey(0, 1, MOD_WIN | MOD_SHIFT, VK_F3);
    RegisterHotKey(0, 2, MOD_WIN | MOD_SHIFT, VK_F4);

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
        DWORD dwVk = HIWORD(msg.lParam);
        FLOAT fLevel = 0.0f;
        BOOL bMute = FALSE;

        switch (dwVk)
        {
        case VK_F1:
            pIAudioEndpointVolumeRender->GetMute(&bMute);
            pIAudioEndpointVolumeRender->SetMute(!bMute, NULL);
            if (bMute)
                PlaySoundW(L"C:\\Windows\\Media\\Speech Sleep.wav", NULL, SND_ASYNC | SND_FILENAME);
            break;

        case VK_F2:
            pIAudioEndpointVolumeCapture->GetMute(&bMute);
            PlaySoundW(bMute ? L"C:\\Windows\\Media\\Speech On.wav" : L"C:\\Windows\\Media\\Speech Off.wav", NULL, SND_ASYNC | SND_FILENAME);
            pIAudioEndpointVolumeCapture->SetMute(!bMute, NULL);
            break;

        case VK_F3:
        case VK_F4:
            pIAudioEndpointVolumeRender->GetMasterVolumeLevelScalar(&fLevel);
            pIAudioEndpointVolumeRender->SetMasterVolumeLevelScalar((fLevel += (dwVk == VK_F3 ? 1 : -1) * 0.01f), NULL);
            PlaySoundW(L"C:\\Windows\\Media\\Windows Notify.wav", NULL, SND_ASYNC | SND_FILENAME);
            break;
        }
    }

    return 0;
}