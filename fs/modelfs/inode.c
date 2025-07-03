// SPDX-License-Identifier: GPL-2.0
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/pagemap.h>
#include <linux/magic.h>
#include <linux/module.h>
#include <linux/fs_context.h>
#include <linux/ai_core.h>

#define MODELFS_MAGIC 0x4d4f444c /* 'MODL' */

static char audit_buf[4096];
static size_t audit_len;

static ssize_t audit_read(struct file *file, char __user *buf, size_t len,
                         loff_t *ppos)
{
    return simple_read_from_buffer(buf, len, ppos, audit_buf, audit_len);
}

static ssize_t audit_write(struct file *file, const char __user *buf, size_t len,
                          loff_t *ppos)
{
    if (len > sizeof(audit_buf) - audit_len)
        len = sizeof(audit_buf) - audit_len;
    if (copy_from_user(audit_buf + audit_len, buf, len))
        return -EFAULT;
    audit_len += len;
    return len;
}

static const struct file_operations audit_fops = {
    .read  = audit_read,
    .write = audit_write,
    .llseek = default_llseek,
};

static const struct tree_descr modelfs_files[] = {
    {"audit.log", &audit_fops, 0644},
    {"", NULL, 0}
};

static int modelfs_fill_super(struct super_block *sb, struct fs_context *fc)
{
    int err;
    err = simple_fill_super(sb, MODELFS_MAGIC, modelfs_files);
    return err;
}

static int modelfs_get_tree(struct fs_context *fc)
{
    return get_tree_single(fc, modelfs_fill_super);
}

static const struct fs_context_operations modelfs_context_ops = {
    .get_tree = modelfs_get_tree,
};

static int modelfs_init_fs_context(struct fs_context *fc)
{
    fc->ops = &modelfs_context_ops;
    return 0;
}

static struct file_system_type modelfs_fs_type = {
    .name           = "modelfs",
    .owner          = THIS_MODULE,
    .init_fs_context = modelfs_init_fs_context,
    .kill_sb        = kill_litter_super,
    .fs_flags       = FS_USERNS_MOUNT,
};
MODULE_ALIAS_FS("modelfs");

static int __init init_modelfs(void)
{
    return register_filesystem(&modelfs_fs_type);
}
module_init(init_modelfs);

static void __exit exit_modelfs(void)
{
    unregister_filesystem(&modelfs_fs_type);
}
module_exit(exit_modelfs);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AINUX");
MODULE_DESCRIPTION("AINUX model filesystem");
