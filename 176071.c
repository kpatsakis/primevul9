static int segment_info_open_fs(struct inode *inode, struct file *file)
{
	return single_open(file, segment_info_seq_show, PDE_DATA(inode));
}