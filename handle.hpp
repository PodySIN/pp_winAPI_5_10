#ifndef HANDLE_HPP
#define HANDLE_HPP

#include <windows.h>

struct Handle {
  HANDLE h;

  explicit Handle(HANDLE x = nullptr):
    h(x)
  {}

  ~Handle()
  {
    close();
  }

  Handle(const Handle&) = delete;
  Handle& operator=(const Handle&) = delete;

  HANDLE get() const
  {
    return h;
  }

  void close()
  {
    if (h && h != INVALID_HANDLE_VALUE) {
      ::CloseHandle(h);
    }
    h = nullptr;
  }
};

#endif
