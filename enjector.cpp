#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <commdlg.h>
#include <vector>
#include <string>

using namespace std;

// Selim'in "Her Seyi Secmeli" Ultra Enjektoru
int main() {
    char dllPath[MAX_PATH] = "";
    OPENFILENAME ofn;
    DWORD procId = 0;

    // 1. ADIM: DLL SECIMI
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = dllPath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "DLL Dosyalari (.dll)\0.dll\0";
    ofn.lpstrTitle = "Enjekte Edilecek DLL Dosyasini Sec";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    cout << "=== SELIM'S ULTRA INJECTOR ===" << endl;
    if (!GetOpenFileName(&ofn)) {
        cout << "Dosya secilmedi, cikiliyor." << endl;
        return 0;
    }

    // 2. ADIM: OYUN/EXE SECIMI
    cout << "\nCalisan Uygulamalar Listeleniyor...\n" << endl;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);
        if (Process32First(hSnap, &procEntry)) {
            do {
                // Sadece onemli exe'leri listele (istege bagli filtreleme yapilabilir)
                cout << "[ID: " << procEntry.th32ProcessID << "] " << procEntry.szExeFile << endl;
            } while (Process32Next(hSnap, &procEntry));
        }
    }
    CloseHandle(hSnap);

    cout << "\nHangi ID'ye enjekte edilsin? (ID numarasini yaz): ";
    cin >> procId;

    // 3. ADIM: ENJEKTE ET
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
    if (hProc) {
        void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, 0);
        HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);
        if (hThread) {
            cout << "\n[+] Basarili! Secilen DLL, ID: " << procId << " nolu isleme basildi." << endl;
            CloseHandle(hThread);
        }
        CloseHandle(hProc);
    } else {
        cout << "\n[-] Hata: Isleme erisilemedi. Yonetici olarak calistirdigindan emin ol!" << endl;
    }

    cout << "\nKapatmak icin Enter'a bas..." << endl;
    system("pause > nul");
    return 0;
}
