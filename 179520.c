guid_to_str(wmem_allocator_t *scope, const e_guid_t *guid)
{
	gchar *buf;

	buf=(gchar *)wmem_alloc(scope, GUID_STR_LEN);
	return guid_to_str_buf(guid, buf, GUID_STR_LEN);
}