void ext4_error_file(struct file *file, const char *function,
		     unsigned int line, ext4_fsblk_t block,
		     const char *fmt, ...)
{
	va_list args;
	struct va_format vaf;
	struct ext4_super_block *es;
	struct inode *inode = file->f_dentry->d_inode;
	char pathname[80], *path;

	es = EXT4_SB(inode->i_sb)->s_es;
	es->s_last_error_ino = cpu_to_le32(inode->i_ino);
	save_error_info(inode->i_sb, function, line);
	path = d_path(&(file->f_path), pathname, sizeof(pathname));
	if (IS_ERR(path))
		path = "(unknown)";
	va_start(args, fmt);
	vaf.fmt = fmt;
	vaf.va = &args;
	if (block)
		printk(KERN_CRIT
		       "EXT4-fs error (device %s): %s:%d: inode #%lu: "
		       "block %llu: comm %s: path %s: %pV\n",
		       inode->i_sb->s_id, function, line, inode->i_ino,
		       block, current->comm, path, &vaf);
	else
		printk(KERN_CRIT
		       "EXT4-fs error (device %s): %s:%d: inode #%lu: "
		       "comm %s: path %s: %pV\n",
		       inode->i_sb->s_id, function, line, inode->i_ino,
		       current->comm, path, &vaf);
	va_end(args);

	ext4_handle_error(inode->i_sb);
}