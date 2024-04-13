static loff_t mdesc_llseek(struct file *file, loff_t offset, int whence)
{
	struct mdesc_handle *hp = file->private_data;

	return no_seek_end_llseek_size(file, offset, whence, hp->handle_size);
}