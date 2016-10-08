#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif

#include "window.h"

int main ( int argc, char** argv )
{
    Window window = Window(800, 600, std::string("Software Renderer"), 60);

}
