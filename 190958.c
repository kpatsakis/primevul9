   Parses an address string */
PHP_FUNCTION(imap_rfc822_parse_adrlist)
{
	zval tovals;
	zend_string *str, *defaulthost;
	char *str_copy;
	ADDRESS *addresstmp;
	ENVELOPE *env;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &str, &defaulthost) == FAILURE) {
		return;
	}

	env = mail_newenvelope();

	/* rfc822_parse_adrlist() modifies passed string. Copy it. */
	str_copy = estrndup(ZSTR_VAL(str), ZSTR_LEN(str));
	rfc822_parse_adrlist(&env->to, str_copy, ZSTR_VAL(defaulthost));
	efree(str_copy);

	array_init(return_value);

	addresstmp = env->to;

	if (addresstmp) do {
		object_init(&tovals);
		if (addresstmp->mailbox) {
			add_property_string(&tovals, "mailbox", addresstmp->mailbox);
		}
		if (addresstmp->host) {
			add_property_string(&tovals, "host", addresstmp->host);
		}
		if (addresstmp->personal) {
			add_property_string(&tovals, "personal", addresstmp->personal);
		}
		if (addresstmp->adl) {
			add_property_string(&tovals, "adl", addresstmp->adl);
		}
		add_next_index_object(return_value, &tovals);
	} while ((addresstmp = addresstmp->next));

	mail_free_envelope(&env);