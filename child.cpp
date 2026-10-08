#include <windows.h>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include "handle.hpp"

bool read_all(DWORD& err, HANDLE rd, std::string& out)
{
  char buf[4096];
  while (true) {
    DWORD n = 0;
    if (!ReadFile(rd, buf, sizeof(buf), &n, nullptr)) {
      DWORD e = GetLastError();
      if (e == ERROR_BROKEN_PIPE) {
        return true;
      }
      err = e;
      return false;
    }
    if (n == 0) {
      return true;
    }
    out.append(buf, n);
  }
}

int main(int argc, char** argv)
{
  if (argc != 2) {
    std::cerr << "No handle\n";
    return 1;
  }

  char* end = nullptr;
  unsigned long long v = std::strtoull(argv[1], &end, 10);
  if (end == argv[1] || *end != '\0' || v == 0) {
    std::cerr << "Bad handle\n";
    return 1;
  }
  Handle rd(reinterpret_cast<HANDLE>(static_cast<std::uintptr_t>(v)));

  std::string msg;
  DWORD err = 0;
  if (!read_all(err, rd.get(), msg)) {
    std::cerr << err << "\n";
    return 1;
  }

  std::cout << msg << "\n";
  std::cout.flush();
  if (!std::cout) {
    std::cerr << "Cant write\n";
    return 1;
  }
  return 0;
}
