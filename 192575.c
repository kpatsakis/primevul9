coolkey_get_attribute_count(const u8 *obj, u8 object_record_type, size_t buf_len)
{
	if (object_record_type == COOLKEY_V0_OBJECT) {
		return coolkey_v0_get_attribute_count(obj, buf_len);
	}
	if (object_record_type != COOLKEY_V1_OBJECT) {
		return 0;
	}
	return coolkey_v1_get_attribute_count(obj, buf_len);
}