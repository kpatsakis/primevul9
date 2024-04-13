static ssize_t bad_file_read(struct file *filp, char __user *buf,
			size_t size, loff_t *ppos)
{
        return -EIO;
}