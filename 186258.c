static int php_session_start_set_ini(zend_string *varname, zend_string *new_value) {
	int ret;
	smart_str buf ={0};
	smart_str_appends(&buf, "session");
	smart_str_appendc(&buf, '.');
	smart_str_append(&buf, varname);
	smart_str_0(&buf);
	ret = zend_alter_ini_entry_ex(buf.s, new_value, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0);
	smart_str_free(&buf);
	return ret;
}