static int calc_dimension(const char* str)
{
	int i = 1;
	while (*str != ']' && *str != '\0') {
		if (*str == ',') {
    		i++;
		}
		str++;
	}
	return i;
}