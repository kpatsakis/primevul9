static uint32_t phar_tar_number(char *buf, int len) /* {{{ */
{
	uint32_t num = 0;
	int i = 0;

	while (i < len && buf[i] == ' ') {
		++i;
	}

	while (i < len && buf[i] >= '0' && buf[i] <= '7') {
		num = num * 8 + (buf[i] - '0');
		++i;
	}

	return num;
}