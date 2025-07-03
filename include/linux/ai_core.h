#ifndef _LINUX_AI_CORE_H
#define _LINUX_AI_CORE_H

#include <linux/types.h>

struct ai_model {
    char name[32];
    void *binary;
    size_t size;
    u64 loaded_at;
};

int ai_model_load(const char *path);
int ai_model_infer(void *input, void *output);
int ai_model_unload(const char *name);

#endif /* _LINUX_AI_CORE_H */
