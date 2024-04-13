coolkey_v1_get_attribute_count(const u8 *obj, size_t buf_len)
{
	coolkey_object_header_t *object_head = (coolkey_object_header_t *)obj;

	if (buf_len <= sizeof(coolkey_object_header_t)) {
		return 0;
	}
	return bebytes2ushort(object_head->attribute_count);
}