coolkey_attribute_start(const u8 *obj, u8 object_record_type, size_t buf_len)
{
	size_t offset = object_record_type == COOLKEY_V1_OBJECT ? sizeof(coolkey_object_header_t) :
			sizeof(coolkey_v0_object_header_t);

	if ((object_record_type != COOLKEY_V1_OBJECT) && (object_record_type != COOLKEY_V0_OBJECT)) {
		return NULL;
	}
	if (offset > buf_len) {
		return NULL;
	}
	return obj + offset;
}