#include <windows.h>
#include <stdio.h>

// Definição de teclas
#define VK_SNAPSHOT 0x2C

// Variáveis globais
HHOOK hKeyboardHook;

// Protótipo de função de callback do hook de teclado
LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam);

// Função para adicionar o programa à inicialização do Windows
void AddToStartup() {
    HKEY hKey;
    const char* czStartName = "MeuProgramaHook";
    char szPath[MAX_PATH];
    DWORD pathLen = GetModuleFileName(NULL, szPath, MAX_PATH);

    if (RegOpenKey(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey) == ERROR_SUCCESS) {
        RegSetValueEx(hKey, czStartName, 0, REG_SZ, (BYTE*)szPath, pathLen);
        RegCloseKey(hKey);
    }
}

// Ponto de entrada do programa
int main(int argc, char *argv[]) {
    // Libera o console associado ao processo
    FreeConsole();
    
    // Adiciona o programa à inicialização do Windows
    AddToStartup();

    HINSTANCE hInstance = GetModuleHandle(NULL);
    MSG messages;

    // Configura o hook de teclado
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, 0);

    // Loop de mensagem para manter o hook rodando
    while (GetMessage(&messages, NULL, 0, 0)) {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    UnhookWindowsHookEx(hKeyboardHook); // Desinstala o hook quando o programa terminar
    return 0;
}

// Função de callback chamada sempre que uma tecla do teclado é pressionada
LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            // Se a tecla PrintScreen for pressionada
            if (p->vkCode == VK_SNAPSHOT) {
                // PrintScreen pressionada - aqui você pode adicionar sua lógica de autenticação
                // Note que o console foi liberado, então esta mensagem não aparecerá em lugar algum.
                // Considerar escrever em um arquivo de log ou algo do tipo para substituir o printf.
                return 1; // Retornar 1 previne que o evento continue
            }
        }
    }
    // Chama o próximo hook na cadeia
    return CallNextHookEx(hKeyboardHook, code, wParam, lParam);
}

