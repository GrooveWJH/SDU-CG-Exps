#include <ggtest.h>
#include <iostream>
#include <unistd.h>

void PrintCurrentWorkingDirectory()
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        std::cout << "Current working directory: " << cwd << std::endl;
    } else {
        perror("getcwd() error");
    }
}
void PrintCurrentWorkingDirectory();
