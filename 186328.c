PS_SERIALIZER_ENCODE_FUNC(php_serialize) /* {{{ */
{
	smart_str buf = {0};
	php_serialize_data_t var_hash;

	IF_SESSION_VARS() {
		PHP_VAR_SERIALIZE_INIT(var_hash);
		php_var_serialize(&buf, Z_REFVAL(PS(http_session_vars)), &var_hash);
		PHP_VAR_SERIALIZE_DESTROY(var_hash);
	}
	return buf.s;
}