void seq_printf(struct seq_file *m, const char *f, ...)
{
	va_list args;

	va_start(args, f);
	seq_vprintf(m, f, args);
	va_end(args);
}