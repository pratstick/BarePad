#include <windows.h>
#include <commdlg.h>

#define ID_FILE_OPEN 1
#define ID_FILE_SAVE 2

HWND hEdit;

void OpenFile(HWND hwnd) {
    OPENFILENAME ofn;
    char szFile[260] = {0};
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (GetOpenFileName(&ofn)) {
        FILE *file = fopen(ofn.lpstrFile, "r");
        if (file) {
            fseek(file, 0, SEEK_END);
            long size = ftell(file);
            rewind(file);
            
            char *buffer = (char*)malloc(size + 1);
            fread(buffer, 1, size, file);
            buffer[size] = '\0';
            
            SetWindowText(hEdit, buffer);
            free(buffer);
            fclose(file);
        }
    }
}

void SaveFile(HWND hwnd) {
    OPENFILENAME ofn;
    char szFile[260] = {0};
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
    ofn.Flags = OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn)) {
        FILE *file = fopen(ofn.lpstrFile, "w");
        if (file) {
            int length = GetWindowTextLength(hEdit);
            char *buffer = (char*)malloc(length + 1);
            GetWindowText(hEdit, buffer, length + 1);
            fwrite(buffer, 1, length, file);
            free(buffer);
            fclose(file);
        }
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            hEdit = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL |
                                 ES_MULTILINE | ES_AUTOVSCROLL,
                                 0, 0, 500, 400, hwnd, NULL, NULL, NULL);
            
            HMENU hMenu = CreateMenu();
            HMENU hFileMenu = CreateMenu();
            AppendMenu(hFileMenu, MF_STRING, ID_FILE_OPEN, "Open");
            AppendMenu(hFileMenu, MF_STRING, ID_FILE_SAVE, "Save");
            AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, "File");
            SetMenu(hwnd, hMenu);
            break;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_FILE_OPEN: OpenFile(hwnd); break;
                case ID_FILE_SAVE: SaveFile(hwnd); break;
            }
            break;
        case WM_SIZE:
            MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "NotepadApp";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    RegisterClass(&wc);
    HWND hwnd = CreateWindow("NotepadApp", "Simple Notepad", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                             CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, NULL, NULL, hInstance, NULL);
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
