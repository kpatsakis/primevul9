void f2fs_msg(struct super_block *sb, const char *level, const char *fmt, ...)
{
	struct va_format vaf;
	va_list args;

	va_start(args, fmt);
	vaf.fmt = fmt;
	vaf.va = &args;
	printk("%sF2FS-fs (%s): %pV\n", level, sb->s_id, &vaf);
	va_end(args);
}