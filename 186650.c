cli_wm_decrypt_macro(int fd, off_t offset, uint32_t len, unsigned char key)
{
	unsigned char *buff;

	if(len == 0)
		return NULL;

	if(fd < 0)
		return NULL;

	buff = (unsigned char *)cli_malloc(len);
	if(buff == NULL)
		return NULL;

	if(!seekandread(fd, offset, SEEK_SET, buff, len)) {
		free(buff);
		return NULL;
	}
	if(key) {
		unsigned char *p;

		for(p = buff; p < &buff[len]; p++)
			*p ^= key;
	}
	return buff;
}