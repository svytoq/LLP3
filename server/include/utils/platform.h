#if defined(__APPLE__) || defined(__MACH__) || defined(__unix__) || defined(__unix)

#include<unistd.h>
#include<sys/types.h>

#define ftrunc(fd, len) ftruncate(fd, len)

#elif defined(_WIN64)
#include <io.h>
#include <windows.h>

#define ftrunc(fd, len) _chsize_s(fd, len)

#endif //GRAPH_ORIENTED_PLATFORM_H