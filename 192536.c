coolkey_v1_get_object_length(u8 *obj, size_t buf_len)
{
	coolkey_combined_object_header_t *object_head = (coolkey_combined_object_header_t *) obj;
	int attribute_count;
	u8 *current_attribute;
	int j;
	size_t len;

	len = sizeof(coolkey_combined_object_header_t);
	if (buf_len <= len) {
		return buf_len;
	}
	attribute_count = bebytes2ushort(object_head->attribute_count);
	buf_len -= len;

	for (current_attribute = obj + len, j = 0; j < attribute_count; j++) {
		size_t attribute_len = coolkey_v1_get_attribute_record_len(current_attribute, buf_len);

		len += attribute_len;
		current_attribute += attribute_len;
		buf_len -= attribute_len;
	}
	return len;
}