static uint32_t phar_tar_checksum(char *buf, int len) /* {{{ */
{
	uint32_t sum = 0;
	char *end = buf + len;

	while (buf != end) {
		sum += (unsigned char)*buf;
		++buf;
	}
	return sum;
}