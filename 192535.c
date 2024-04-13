coolkey_v1_get_attribute_data(const u8 *attr, size_t buf_len, sc_cardctl_coolkey_attribute_t *attr_out)
{
	int r;
	size_t len;
	coolkey_attribute_header_t *attribute_head = (coolkey_attribute_header_t *)attr;

	if (buf_len < sizeof(coolkey_attribute_header_t)) {
		return SC_ERROR_CORRUPTED_DATA;
	}

	/* we must have type V1. Process according to data type */
	switch (attribute_head->attribute_data_type) {
	/* ULONG has implied length of 4 */
	case COOLKEY_ATTR_TYPE_INTEGER:
		if (buf_len < (sizeof(coolkey_attribute_header_t) + 4)) {
			return SC_ERROR_CORRUPTED_DATA;
		}
		attr_out->attribute_data_type = SC_CARDCTL_COOLKEY_ATTR_TYPE_ULONG;
		attr_out->attribute_length = 4;
		attr_out->attribute_value = attr + sizeof(coolkey_attribute_header_t);
		return SC_SUCCESS;
	/* BOOL_FALSE and BOOL_TRUE have implied length and data */
	/* return type STRING for BOOLS */
	case COOLKEY_ATTR_TYPE_BOOL_FALSE:
		attr_out->attribute_length = 1;
		attr_out->attribute_value =  &coolkey_static_false;
		return SC_SUCCESS;
	case COOLKEY_ATTR_TYPE_BOOL_TRUE:
		attr_out->attribute_length = 1;
		attr_out->attribute_value =  &coolkey_static_true;
		return SC_SUCCESS;
	/* string type has encoded length */
	case COOLKEY_ATTR_TYPE_STRING:
		r = coolkey_v1_get_attribute_len(attr, buf_len, &len, 0);
		if (r < SC_SUCCESS) {
			return r;
		}
		if (buf_len < (len + sizeof(coolkey_attribute_header_t) + 2)) {
			return SC_ERROR_CORRUPTED_DATA;
		}
		attr_out->attribute_value = attr+sizeof(coolkey_attribute_header_t)+2;
		attr_out->attribute_length = len;
		return SC_SUCCESS;
	default:
		break;
	}
	return SC_ERROR_CORRUPTED_DATA;
}