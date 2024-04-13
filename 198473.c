sc_pkcs15emu_add_prkey(sc_pkcs15_card_t *p15card,
                const sc_pkcs15_id_t *id,
                const char *label,
                int type, unsigned int modulus_length, int usage,
                const sc_path_t *path, int ref,
                const sc_pkcs15_id_t *auth_id, int obj_flags)
{
	sc_pkcs15_prkey_info_t *info;

	info = calloc(1, sizeof(*info));
	if (!info)
	{
		LOG_FUNC_RETURN(p15card->card->ctx, SC_ERROR_OUT_OF_MEMORY);
	}
	info->id                = *id;
	info->modulus_length    = modulus_length;
	info->usage             = usage;
	info->native            = 1;
	info->access_flags      = SC_PKCS15_PRKEY_ACCESS_SENSITIVE
                                | SC_PKCS15_PRKEY_ACCESS_ALWAYSSENSITIVE
                                | SC_PKCS15_PRKEY_ACCESS_NEVEREXTRACTABLE
                                | SC_PKCS15_PRKEY_ACCESS_LOCAL;
	info->key_reference     = ref;

	if (path)
		info->path = *path;

	return sc_pkcs15emu_add_object(p15card, type, label,
			info, auth_id, obj_flags);
}