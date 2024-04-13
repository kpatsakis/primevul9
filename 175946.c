static int* get_position_12(int dimension, const char* str)
{
	int *pos;
	int i = -1, flag = 0;

	pos = safe_emalloc(sizeof(int), dimension, 0);
	memset(pos,0,sizeof(int)*dimension);
	while (*str != '\0' && (*str < '0' || *str > '9') && (*str != '*')) {
		str++;
	}
	if (*str == '*') {
		str++;
		i++;
	}
	while (*str != '\0') {
		if (*str >= '0' && *str <= '9') {
			if (flag == 0) {
				i++;
				flag = 1;
			}
			pos[i] = (pos[i]*10)+(*str-'0');
		} else if (*str == '*') {
			soap_error0(E_ERROR, "Encoding: '*' may only be first arraySize value in list");
		} else {
		  flag = 0;
		}
		str++;
	}
	return pos;
}