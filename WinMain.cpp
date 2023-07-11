
#include <initguid.h>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#define printf __builtin_printf

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

        if (dwModifiers == (MOD_WIN | MOD_CONTROL))
        {
            pIAudioEndpointVolumeRender->GetMasterVolumeLevelScalar(&fLevel);
            switch (dwVk)
            {
            case VK_OEM_PLUS:
                fLevel += 0.01f;
                break;
            case VK_OEM_MINUS:
                fLevel -= 0.01f;
                break;
            }
            pIAudioEndpointVolumeRender->SetMasterVolumeLevelScalar(fLevel, NULL);
            PlaySoundW(L"C:\\Windows\\Media\\Windows Notify.wav", NULL, SND_ASYNC | SND_FILENAME);
        }
        else if (dwVk == 0x4D)
        {
            switch (dwModifiers)
            {
            case MOD_WIN | MOD_CONTROL | MOD_ALT:
                pIAudioEndpointVolumeRender->GetMute(&bMute);
                pIAudioEndpointVolumeRender->SetMute(!bMute, NULL);
                if (bMute)
                    PlaySoundW(L"C:\\Windows\\Media\\Speech Sleep.wav", NULL, SND_ASYNC | SND_FILENAME);
                break;

            case MOD_WIN | MOD_CONTROL | MOD_SHIFT:
                pIAudioEndpointVolumeCapture->GetMute(&bMute);
                PlaySoundW(bMute ? L"C:\\Windows\\Media\\Speech On.wav" : L"C:\\Windows\\Media\\Speech Off.wav", NULL, SND_ASYNC | SND_FILENAME);
                pIAudioEndpointVolumeCapture->SetMute(!bMute, NULL);
                break;
            }
        }
    }

    return 0;
}