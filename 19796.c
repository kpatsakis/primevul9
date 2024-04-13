void seq_escape(struct seq_file *m, const char *s, const char *esc)
{
	seq_escape_str(m, s, ESCAPE_OCTAL, esc);
}