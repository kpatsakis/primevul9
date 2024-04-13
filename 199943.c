static int bad_file_readdir(struct file *filp, void *dirent, filldir_t filldir)
{
	return -EIO;
}