PHPAPI int php_session_register_serializer(const char *name, zend_string *(*encode)(PS_SERIALIZER_ENCODE_ARGS), int (*decode)(PS_SERIALIZER_DECODE_ARGS)) /* {{{ */
{
	int ret = FAILURE;
	int i;

	for (i = 0; i < MAX_SERIALIZERS; i++) {
		if (ps_serializers[i].name == NULL) {
			ps_serializers[i].name = name;
			ps_serializers[i].encode = encode;
			ps_serializers[i].decode = decode;
			ps_serializers[i + 1].name = NULL;
			ret = SUCCESS;
			break;
		}
	}
	return ret;
}