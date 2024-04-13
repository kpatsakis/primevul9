coolkey_v1_get_attribute_len(const u8 *attr, size_t buf_len, size_t *len, int encoded_len)
{
	coolkey_attribute_header_t *attribute_head = (coolkey_attribute_header_t *)attr;

	*len = 0;
	/* don't reference beyond our buffer */
	if (buf_len < sizeof(coolkey_attribute_header_t)) {
		return SC_ERROR_CORRUPTED_DATA;
	}
	switch (attribute_head->attribute_data_type) {
	case COOLKEY_ATTR_TYPE_STRING:
		if (buf_len < (sizeof(coolkey_attribute_header_t) +2)) {
			break;
		}
		*len = bebytes2ushort(attr + sizeof(coolkey_attribute_header_t));
		if (encoded_len) {
			*len += 2;
		}
		return SC_SUCCESS;
	case COOLKEY_ATTR_TYPE_BOOL_FALSE:
	case COOLKEY_ATTR_TYPE_BOOL_TRUE:
		/* NOTE: there is no encoded data from TYPE_BOOL_XXX, so we return length 0, but the length
		 * of the attribute is actually 1 byte, so if encoded_len == false, return 1 */
		*len = encoded_len ? 0: 1;
		return SC_SUCCESS;
		break;
	case COOLKEY_ATTR_TYPE_INTEGER:
		*len = 4; /* length is 4 in both encoded length and attribute length */
		return SC_SUCCESS;
	default:
		break;
	}
	return SC_ERROR_CORRUPTED_DATA;
}