static ssize_t bad_file_write(struct file *filp, const char __user *buf,
			size_t siz, loff_t *ppos)
{
        return -EIO;
}