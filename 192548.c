coolkey_v1_get_attribute_record_len(const u8 *attr, size_t buf_len)
{
	size_t attribute_len = sizeof(coolkey_attribute_header_t);
	size_t len = 0;
	int r;

	r = coolkey_v1_get_attribute_len(attr, buf_len, &len, 1);
	if (r < 0) {
		return buf_len; /* skip to the end, ignore the rest of the record */
	}

	return MIN(buf_len,attribute_len+len);
}