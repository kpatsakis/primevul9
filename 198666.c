acl_to_ac_byte(struct sc_card *card, const struct sc_acl_entry *e)
{
	if (e == NULL)
		return SC_ERROR_OBJECT_NOT_FOUND;

	switch (e->method) {
	case SC_AC_NONE:
		LOG_FUNC_RETURN(card->ctx, EPASS2003_AC_MAC_NOLESS | EPASS2003_AC_EVERYONE);
	case SC_AC_NEVER:
		LOG_FUNC_RETURN(card->ctx, EPASS2003_AC_MAC_NOLESS | EPASS2003_AC_NOONE);
	default:
		LOG_FUNC_RETURN(card->ctx, EPASS2003_AC_MAC_NOLESS | EPASS2003_AC_USER);
	}

	LOG_FUNC_RETURN(card->ctx, SC_ERROR_INCORRECT_PARAMETERS);
}