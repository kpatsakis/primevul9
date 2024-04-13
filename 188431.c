static inline int unhex(int a)
{
	if (a >= 'A' && a <= 'F') return a - 'A' + 0xA;
	if (a >= 'a' && a <= 'f') return a - 'a' + 0xA;
	if (a >= '0' && a <= '9') return a - '0';
	return 0;
}