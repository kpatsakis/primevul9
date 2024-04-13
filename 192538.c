coolkey_get_attribute_type(const u8 *attr, u8 obj_record_type, size_t buf_len)
{
	coolkey_attribute_header_t *attribute_header = (coolkey_attribute_header_t *) attr;

	return bebytes2ulong(attribute_header->attribute_attr_type);
}