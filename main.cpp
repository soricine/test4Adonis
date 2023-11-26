#include <iostream>
#include <string>
#include <Windows.h>
#include <ShlObj_core.h>
#include <Shobjidl.h>
#include <Objbase.h>
#include <Pathcch.h>

class ShortcutCreator {
public:
    ShortcutCreator(const wchar_t* targetPath, const wchar_t* shortcutName)
        : targetPath_(targetPath), shortcutName_(shortcutName) {}

    void CreateShortcut() {
        try {
            InitializeCOM();

            CComPtr<IShellLink> pShellLink;
            CComPtr<IPersistFile> pPersistFile;

            CreateShellLink(&pShellLink);
            SetTargetPath(pShellLink);
            QueryPersistFileInterface(pShellLink, &pPersistFile);

            WCHAR desktopPath[MAX_PATH];
            GetDesktopPath(desktopPath);

            WCHAR shortcutPath[MAX_PATH];
            CombinePath(desktopPath, shortcutName_, shortcutPath);

            SaveShortcut(pPersistFile, shortcutPath);

            std::wcout << L"Shortcut created: " << shortcutPath << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }

        UninitializeCOM();
    }

private:
    const wchar_t* targetPath_;
    const wchar_t* shortcutName_;

    void InitializeCOM() {
        HRESULT hr = CoInitialize(nullptr);
        if (FAILED(hr)) {
            throw std::runtime_error("CoInitialize failed");
        }
    }

    void CreateShellLink(IShellLink** ppShellLink) {
        HRESULT hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)ppShellLink);
        if (FAILED(hr)) {
            throw std::runtime_error("CoCreateInstance failed for IShellLink");
        }
    }

    void SetTargetPath(IShellLink* pShellLink) {
        HRESULT hr = pShellLink->SetPath(targetPath_);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to set target path");
        }
    }

    void QueryPersistFileInterface(IShellLink* pShellLink, IPersistFile** ppPersistFile) {
        HRESULT hr = pShellLink->QueryInterface(IID_IPersistFile, (LPVOID*)ppPersistFile);
        if (FAILED(hr)) {
            throw std::runtime_error("QueryInterface failed for IPersistFile");
        }
    }

    void GetDesktopPath(WCHAR* desktopPath) {
        HRESULT hr = SHGetFolderPath(nullptr, CSIDL_DESKTOP, nullptr, 0, desktopPath);
        if (FAILED(hr)) {
            throw std::runtime_error("SHGetFolderPath failed");
        }
    }

    void CombinePath(const WCHAR* basePath, const WCHAR* relativePath, WCHAR* resultPath) {
        HRESULT hr = PathCchCombine(resultPath, MAX_PATH, basePath, relativePath);
        if (FAILED(hr)) {
            throw std::runtime_error("PathCchCombine failed");
        }
    }

    void SaveShortcut(IPersistFile* pPersistFile, const WCHAR* shortcutPath) {
        HRESULT hr = pPersistFile->Save(shortcutPath, TRUE);
        if (FAILED(hr)) {
            throw std::runtime_error("Save failed");
        }
    }

    void UninitializeCOM() {
        CoUninitialize();
    }
};

int main() {
    // Specify the path to Notepad and the name for the shortcut
    const wchar_t* targetPath = L"C:\\Windows\\System32\\notepad.exe";
    const wchar_t* shortcutName = L"OpenNotepad.lnk";

    // Create the shortcut using the ShortcutCreator class
    ShortcutCreator shortcutCreator(targetPath, shortcutName);
    shortcutCreator.CreateShortcut();

    return 0;
}