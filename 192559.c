coolkey_v0_get_attribute_count(const u8 *obj, size_t buf_len)
{
	coolkey_v0_object_header_t *object_head = (coolkey_v0_object_header_t *)obj;
	const u8 *attr;
	int count = 0;
	size_t attribute_data_len;

	/* make sure we have enough of the object to read the record_type */
	if (buf_len <= sizeof(coolkey_v0_object_header_t)) {
		return 0;
	}
	/*
	 * now loop through all the attributes in the list. first find the start of the list
	 */
	attr = coolkey_attribute_start(obj, COOLKEY_V0_OBJECT, buf_len);
	if (attr == NULL) {
		return 0;
	}

	buf_len -= (attr-obj);
	attribute_data_len = bebytes2ushort(object_head->attribute_data_len);
	if (buf_len < attribute_data_len) {
		return 0;
	}

	while (attribute_data_len) {
		size_t len = coolkey_v0_get_attribute_record_len(attr, buf_len);

		if (len == 0) {
			break;
		}
		/*  This is an error in the token data, don't parse the last attribute */
		if (len > attribute_data_len) {
			break;
		}
		/* we know that coolkey_v0_get_attribute_record_len never
		 * 	returns more than buf_len, so we can safely assert that.
		 * 	If the assert is true, you can easily see that the loop
		 * 	will eventually break with len == 0, even if attribute_data_len
		 * 	was invalid */
		assert(len <= buf_len);
		count++;
		attr += len;
		buf_len -= len;
		attribute_data_len -= len;
	}
	return count;
}