void seq_put_decimal_ull(struct seq_file *m, const char *delimiter,
			 unsigned long long num)
{
	return seq_put_decimal_ull_width(m, delimiter, num, 0);
}