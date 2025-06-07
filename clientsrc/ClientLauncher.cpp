#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <cstring>

#ifdef _MSC_VER
  #include <intrin.h>
#else
  #include <cpuid.h>
#endif

std::string get_cpu_vendor() {
    std::array<int, 4> cpuInfo{};
    // EAX set to 0 -> vendor string
#ifdef _MSC_VER
    __cpuid(cpuInfo.data(), 0);
#else
    __cpuid(0, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
#endif
    char vendor[13];
    std::memcpy(vendor, &cpuInfo[1], 4); // EBX
    std::memcpy(vendor + 4, &cpuInfo[3], 4); // EDX
    std::memcpy(vendor + 8, &cpuInfo[2], 4); // ECX
    vendor[12] = '\0';

    return std::string(vendor);
}

std::string get_exe_directory() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string full_path(path);
    size_t last_slash = full_path.find_last_of("\\/");
    return full_path.substr(0, last_slash);
}


// reference: https://learn.microsoft.com/de-de/windows/win32/procthread/creating-processes?redirectedfrom=MSDN
int launch(const std::string& relative_path) {
    std::string exe_path = get_exe_directory() + "\\" + relative_path;

    std::string cmd = "\"" + exe_path + "\"";
    std::vector<char> cmdline(cmd.begin(), cmd.end());
    cmdline.push_back('\0');

    std::string working_dir = get_exe_directory() + "\\bin";

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi = {};

    std::cout << "Launching: " << exe_path << "\n";
    if (!CreateProcessA(
        NULL,
        cmdline.data(),
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        working_dir.c_str(), // Set working dir
        &si,
        &pi))
    {
        std::cerr << "CreateProcess failed (" << GetLastError() << ").\n";
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}

int main() {
    std::string vendor = get_cpu_vendor();
    std::string exe;

    if (vendor == "GenuineIntel") {
        std::cout << "Detected Intel CPU. Launching Intel version..." << std::endl;
        exe = "bin\\client_tower_guard_intel.exe";
    } else if (vendor == "AuthenticAMD") {
        std::cout << "Detected AMD CPU. Launching AMD version..." << std::endl;
        exe = "bin\\client_tower_guard_amd.exe";
    } else {
        std::cout << "Unknown CPU vendor (" << vendor << "). Launching generic version..." << std::endl;
        exe = "bin\\client_tower_guard_generic.exe";
    }

    return launch(exe);
}