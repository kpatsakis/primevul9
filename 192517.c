static char * coolkey_get_manufacturer(coolkey_cuid_t *cuid)
{
	unsigned short fabricator = bebytes2ushort(cuid->ic_fabricator);
	int i;
	char *buf;
	const char *manufacturer_string = "%04x Unknown";
	size_t len;
	int r;

	for (i=0; i < manufacturer_list_count; i++) {
		if (manufacturer_list[i].id == fabricator) {
			manufacturer_string = manufacturer_list[i].string;
			break;
		}
	}
	len = strlen(manufacturer_string)+1;
	buf= malloc(len);
	if (buf == NULL) {
		return NULL;
	}
	r = snprintf(buf, len, manufacturer_string, fabricator);
	if (r < 0) {
		free(buf);
		return NULL;
	}
	return buf;
}