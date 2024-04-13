int nonseekable_open(struct inode *inode, struct file *filp)
{
	filp->f_mode &= ~(FMODE_LSEEK | FMODE_PREAD | FMODE_PWRITE);
	return 0;
}