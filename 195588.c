static int options_seq_show(struct seq_file *seq, void *offset)
{
	struct super_block *sb = seq->private;
	int rc;

	seq_puts(seq, (sb->s_flags & MS_RDONLY) ? "ro" : "rw");
	rc = _ext4_show_options(seq, sb, 1);
	seq_puts(seq, "\n");
	return rc;
}