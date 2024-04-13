get_unicode_name(const char *name, int size, int big_endian)
{
	int i, increment;
	char *newname, *ret;

	if((name == NULL) || (*name == '\0') || (size <= 0))
		return NULL;

	newname = (char *)cli_malloc(size * 7 + 1);
	if(newname == NULL)
		return NULL;

	if((!big_endian) && (size & 0x1)) {
		cli_dbgmsg("get_unicode_name: odd number of bytes %d\n", size);
		--size;
	}

	increment = (big_endian) ? 1 : 2;
	ret = newname;

	for(i = 0; i < size; i += increment) {
		if((!(name[i]&0x80)) && isprint(name[i])) {
		        *ret++ = tolower(name[i]);
		} else {
			if((name[i] < 10) && (name[i] >= 0)) {
				*ret++ = '_';
				*ret++ = (char)(name[i] + '0');
			} else {
				const uint16_t x = (uint16_t)((name[i] << 8) | name[i + 1]);

				*ret++ = '_';
				*ret++ = (char)('a'+((x&0xF)));
				*ret++ = (char)('a'+((x>>4)&0xF));
				*ret++ = (char)('a'+((x>>8)&0xF));
				*ret++ = 'a';
				*ret++ = 'a';
			}
			*ret++ = '_';
		}
	}

	*ret = '\0';

	/* Saves a lot of memory */
	ret = cli_realloc(newname, (ret - newname) + 1);
	return ret ? ret : newname;
}