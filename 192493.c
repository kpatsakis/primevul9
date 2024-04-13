static int sg_proc_open_devstrs(struct inode *inode, struct file *file)
{
        return seq_open(file, &devstrs_seq_ops);
}