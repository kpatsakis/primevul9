loff_t seq_lseek(struct file *file, loff_t offset, int whence)
{
	struct seq_file *m = file->private_data;
	loff_t retval = -EINVAL;

	mutex_lock(&m->lock);
	switch (whence) {
	case SEEK_CUR:
		offset += file->f_pos;
		fallthrough;
	case SEEK_SET:
		if (offset < 0)
			break;
		retval = offset;
		if (offset != m->read_pos) {
			while ((retval = traverse(m, offset)) == -EAGAIN)
				;
			if (retval) {
				/* with extreme prejudice... */
				file->f_pos = 0;
				m->read_pos = 0;
				m->index = 0;
				m->count = 0;
			} else {
				m->read_pos = offset;
				retval = file->f_pos = offset;
			}
		} else {
			file->f_pos = offset;
		}
	}
	mutex_unlock(&m->lock);
	return retval;
}