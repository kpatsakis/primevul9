PS_SERIALIZER_ENCODE_FUNC(php_binary) /* {{{ */
{
	smart_str buf = {0};
	php_serialize_data_t var_hash;
	PS_ENCODE_VARS;

	PHP_VAR_SERIALIZE_INIT(var_hash);

	PS_ENCODE_LOOP(
			if (ZSTR_LEN(key) > PS_BIN_MAX) continue;
			smart_str_appendc(&buf, (unsigned char)ZSTR_LEN(key));
			smart_str_appendl(&buf, ZSTR_VAL(key), ZSTR_LEN(key));
			php_var_serialize(&buf, struc, &var_hash);
	);

	smart_str_0(&buf);
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	return buf.s;
}