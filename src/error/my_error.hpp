#ifndef MY_ERROR_HPP
#define MY_ERROR_HPP
#include <stdio.h>
#include <stdlib.h>
namespace protocol
{
    namespace error
    {
        void handle_void(const char *ebuff)
        {
            perror(ebuff);
            printf("\n");
            exit(-1);
        }
        int handle_int(const char *ebuff)
        {
            perror(ebuff);
            printf("\n");
            return (-1);
        }
    }
}

#endif