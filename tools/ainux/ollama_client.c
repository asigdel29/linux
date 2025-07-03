// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>

/* Simple helper to call the local Ollama API for inference */
int ollama_infer(const char *model, const char *prompt)
{
    char cmd[512];

    if (!model || !prompt)
        return -1;

    snprintf(cmd, sizeof(cmd),
             "curl -s http://localhost:11434/api/generate -d '{\"model\":\"%s\",\"prompt\":\"%s\"}'",
             model, prompt);
    return system(cmd);
}
