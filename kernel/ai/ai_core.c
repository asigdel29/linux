// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/timekeeping.h>
#include <linux/ai_core.h>
#include <linux/list.h>
#include <linux/string.h>

static LIST_HEAD(model_list);
static DEFINE_MUTEX(model_lock);
static struct proc_dir_entry *ai_dir;
static struct proc_dir_entry *ai_models_entry;
static struct proc_dir_entry *ai_mem_entry;

static int models_show(struct seq_file *m, void *v)
{
    struct ai_model *model;

    mutex_lock(&model_lock);
    list_for_each_entry(model, &model_list, list) {
        seq_printf(m, "%s %zu bytes loaded_at %llu infers %llu\n",
                   model->name, model->size, model->loaded_at,
                   model->metrics.inference_count);
    }
    mutex_unlock(&model_lock);
    return 0;
}

static int models_open(struct inode *inode, struct file *file)
{
    return single_open(file, models_show, NULL);
}

static const struct proc_ops ai_models_proc_ops = {
    .proc_open    = models_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

static int mem_show(struct seq_file *m, void *v)
{
    struct ai_model *model;
    size_t total = 0;

    mutex_lock(&model_lock);
    list_for_each_entry(model, &model_list, list)
        total += model->size;
    mutex_unlock(&model_lock);

    seq_printf(m, "%zu\n", total);
    return 0;
}

static int mem_open(struct inode *inode, struct file *file)
{
    return single_open(file, mem_show, NULL);
}

static const struct proc_ops ai_mem_proc_ops = {
    .proc_open    = mem_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

struct ai_model *ai_model_lookup(const char *name)
{
    struct ai_model *m;

    mutex_lock(&model_lock);
    list_for_each_entry(m, &model_list, list) {
        if (sysfs_streq(m->name, name)) {
            mutex_unlock(&model_lock);
            return m;
        }
    }
    mutex_unlock(&model_lock);
    return NULL;
}
EXPORT_SYMBOL_GPL(ai_model_lookup);

int ai_model_load(const char *path)
{
    struct ai_model *m;

    m = kzalloc(sizeof(*m), GFP_KERNEL);
    if (!m)
        return -ENOMEM;

    strscpy(m->name, path, sizeof(m->name));
    m->loaded_at = ktime_get_real_seconds();
    m->binary = (void *)0x1; /* placeholder */
    m->size = 0;

    mutex_lock(&model_lock);
    list_add_tail(&m->list, &model_list);
    mutex_unlock(&model_lock);

    pr_info("ai_core: model %s loaded\n", m->name);
    return 0;
}
EXPORT_SYMBOL_GPL(ai_model_load);

int ai_model_infer(void *input, void *output)
{
    struct ai_model *model;

    mutex_lock(&model_lock);
    model = list_first_entry_or_null(&model_list, struct ai_model, list);
    if (model) {
        model->metrics.inference_count++;
        model->metrics.last_inference_ns = ktime_get_ns();
    }
    mutex_unlock(&model_lock);

    return -ENOSYS; /* real inference not implemented */
}
EXPORT_SYMBOL_GPL(ai_model_infer);

int ai_model_unload(const char *name)
{
    struct ai_model *model;

    mutex_lock(&model_lock);
    list_for_each_entry(model, &model_list, list) {
        if (sysfs_streq(model->name, name)) {
            list_del(&model->list);
            kfree(model);
            pr_info("ai_core: model %s unloaded\n", name);
            mutex_unlock(&model_lock);
            return 0;
        }
    }
    mutex_unlock(&model_lock);
    return 0;
}
EXPORT_SYMBOL_GPL(ai_model_unload);

static int __init ai_core_init(void)
{
    ai_dir = proc_mkdir("ai", NULL);
    if (!ai_dir)
        return -ENOMEM;

    ai_models_entry = proc_create("models", 0444, ai_dir, &ai_models_proc_ops);
    if (!ai_models_entry)
        return -ENOMEM;

    ai_mem_entry = proc_create("mem", 0444, ai_dir, &ai_mem_proc_ops);
    if (!ai_mem_entry)
        return -ENOMEM;

    pr_info("ai_core initialized\n");
    return 0;
}
module_init(ai_core_init);

static void __exit ai_core_exit(void)
{
    proc_remove(ai_mem_entry);
    proc_remove(ai_models_entry);
    proc_remove(ai_dir);
    pr_info("ai_core exit\n");
}
module_exit(ai_core_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AINUX");
MODULE_DESCRIPTION("AINUX AI core module");
