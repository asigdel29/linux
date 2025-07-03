// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "ollama_client.c"

int main(int argc, char *argv[])
{
    printf("ainuxd starting (gRPC mock)\n");

    if (argc > 2 && !strcmp(argv[1], "--infer")) {
        return ollama_infer(argv[2], argc > 3 ? argv[3] : "");
    }

    /* TODO: implement gRPC server */
    return 0;
}
