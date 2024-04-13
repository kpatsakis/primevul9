static inline int is_real_num_char(int c)
{
	return (c >= '0' && c <= '9') ||
		c == 'e' || c == 'E' || c == '+' || c == '-' || c == '.';
}