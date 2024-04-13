static int sg_proc_open_debug(struct inode *inode, struct file *file)
{
        return seq_open(file, &debug_seq_ops);
}