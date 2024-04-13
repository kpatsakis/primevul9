static void pad_len_spaces(struct seq_file *m, int len)
{
	len = 25 + sizeof(void*) * 6 - len;
	if (len < 1)
		len = 1;
	seq_printf(m, "%*c", len, ' ');
}