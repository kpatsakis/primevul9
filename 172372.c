imapx_password_contains_nonascii (CamelService *service)
{
	const gchar *password;

	g_return_val_if_fail (CAMEL_IS_SERVICE (service), FALSE);

	password = camel_service_get_password (service);

	while (password && *password) {
		if (*password < 0)
			return TRUE;

		password++;
	}

	return FALSE;
}