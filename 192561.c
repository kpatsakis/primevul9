coolkey_v0_get_attribute_len(const u8 *attr, size_t buf_len, size_t *len)
{
	coolkey_v0_attribute_header_t *attribute_head = (coolkey_v0_attribute_header_t *)attr;
	/* don't reference beyond our buffer */
	if (buf_len < sizeof(coolkey_v0_attribute_header_t)) {
		return SC_ERROR_CORRUPTED_DATA;
	}
	*len = bebytes2ushort(attribute_head->attribute_data_len);
	return SC_SUCCESS;
}