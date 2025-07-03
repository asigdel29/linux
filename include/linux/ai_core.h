#ifndef _LINUX_AI_CORE_H
#define _LINUX_AI_CORE_H

#include <linux/types.h>
#include <linux/list.h>

struct ai_metrics {
    u64 inference_count;
    u64 last_inference_ns;
};

struct ai_model {
    char name[32];
    void *binary;
    size_t size;
    u64 loaded_at;
    struct ai_metrics metrics;
    struct list_head list;
};

struct ai_model *ai_model_lookup(const char *name);

int ai_model_load(const char *path);
int ai_model_infer(void *input, void *output);
int ai_model_unload(const char *name);

#endif /* _LINUX_AI_CORE_H */
