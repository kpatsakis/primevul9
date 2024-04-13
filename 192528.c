coolkey_get_attribute_record_len(const u8 *attr, u8 obj_record_type, size_t buf_len)
{
	if (obj_record_type ==  COOLKEY_V0_OBJECT) {
		return coolkey_v0_get_attribute_record_len(attr, buf_len);
	}
	if (obj_record_type != COOLKEY_V1_OBJECT) {
		return buf_len; /* skip to the end */
	}
	return coolkey_v1_get_attribute_record_len(attr, buf_len);
}