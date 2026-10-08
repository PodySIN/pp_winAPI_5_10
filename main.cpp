#include <windows.h>
#include <cstdint>
#include <iostream>
#include <string>
#include "handle.hpp"

bool send(DWORD& err, HANDLE wr, const char* b, DWORD k)
{
  DWORD r = 0;
  while (r < k) {
    DWORD n = 0;
    if (!WriteFile(wr, b + r, k - r, &n, nullptr)) {
      err = GetLastError();
      return false;
    }
    r += n;
  }
  return true;
}

int main()
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Cant read line\n";
    return 1;
  }

  SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), nullptr, FALSE};
  HANDLE r = nullptr, w = nullptr;
  if (!CreatePipe(&r, &w, &sa, 0)) {
    std::cerr << GetLastError() << "\n";
    return 1;
  }
  Handle rd(r), wr(w);

  if (!SetHandleInformation(rd.get(), HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT)) {
    std::cerr << GetLastError() << "\n";
    return 1;
  }

  PROCESS_INFORMATION pi = {};
  STARTUPINFOA si = {};
  si.cb = sizeof(si);
  std::string cmd = "child.exe " +
    std::to_string(reinterpret_cast<std::uintptr_t>(rd.get()));

  if (!CreateProcessA(nullptr, &cmd[0], nullptr, nullptr, TRUE,
                      0, nullptr, nullptr, &si, &pi)) {
    std::cerr << GetLastError() << "\n";
    return 1;
  }
  Handle proc(pi.hProcess), thr(pi.hThread);

  rd.close();

  DWORD err = 0;
  bool ok = send(err, wr.get(), line.data(), static_cast<DWORD>(line.size()));
  wr.close();

  if (!ok) {
    std::cerr << err << "\n";
  }

  if (WaitForSingleObject(proc.get(), INFINITE) != WAIT_OBJECT_0) {
    std::cerr << GetLastError() << "\n";
    return 1;
  }

  DWORD code = 1;
  if (!GetExitCodeProcess(proc.get(), &code)) {
    std::cerr << GetLastError() << "\n";
    return 1;
  }
  return (ok && code == 0) ? 0 : 1;
}
