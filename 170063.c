static char* timelib_ltrim(char *s)
{
	char *ptr = s;
	while (ptr[0] == ' ' || ptr[0] == '\t') {
		ptr++;
	}
	return ptr;
}