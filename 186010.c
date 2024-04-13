generic_file_llseek_size(struct file *file, loff_t offset, int whence,
		loff_t maxsize, loff_t eof)
{
	switch (whence) {
	case SEEK_END:
		offset += eof;
		break;
	case SEEK_CUR:
		/*
		 * Here we special-case the lseek(fd, 0, SEEK_CUR)
		 * position-querying operation.  Avoid rewriting the "same"
		 * f_pos value back to the file because a concurrent read(),
		 * write() or lseek() might have altered it
		 */
		if (offset == 0)
			return file->f_pos;
		/*
		 * f_lock protects against read/modify/write race with other
		 * SEEK_CURs. Note that parallel writes and reads behave
		 * like SEEK_SET.
		 */
		spin_lock(&file->f_lock);
		offset = vfs_setpos(file, file->f_pos + offset, maxsize);
		spin_unlock(&file->f_lock);
		return offset;
	case SEEK_DATA:
		/*
		 * In the generic case the entire file is data, so as long as
		 * offset isn't at the end of the file then the offset is data.
		 */
		if (offset >= eof)
			return -ENXIO;
		break;
	case SEEK_HOLE:
		/*
		 * There is a virtual hole at the end of the file, so as long as
		 * offset isn't i_size or larger, return i_size.
		 */
		if (offset >= eof)
			return -ENXIO;
		offset = eof;
		break;
	}

	return vfs_setpos(file, offset, maxsize);
}