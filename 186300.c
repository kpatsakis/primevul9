PS_SERIALIZER_ENCODE_FUNC(php) /* {{{ */
{
	smart_str buf = {0};
	php_serialize_data_t var_hash;
	PS_ENCODE_VARS;

	PHP_VAR_SERIALIZE_INIT(var_hash);

	PS_ENCODE_LOOP(
		smart_str_appendl(&buf, ZSTR_VAL(key), ZSTR_LEN(key));
		if (memchr(ZSTR_VAL(key), PS_DELIMITER, ZSTR_LEN(key))) {
			PHP_VAR_SERIALIZE_DESTROY(var_hash);
			smart_str_free(&buf);
			return NULL;
		}
		smart_str_appendc(&buf, PS_DELIMITER);
		php_var_serialize(&buf, struc, &var_hash);
	);

	smart_str_0(&buf);

	PHP_VAR_SERIALIZE_DESTROY(var_hash);
	return buf.s;
}