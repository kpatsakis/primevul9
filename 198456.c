sc_pkcs15emu_add_cert(sc_pkcs15_card_t *p15card,
		int type, int authority,
		const sc_path_t *path,
		const sc_pkcs15_id_t *id,
                const char *label, int obj_flags)
{
	sc_pkcs15_cert_info_t *info;
	info = calloc(1, sizeof(*info));
	if (!info)
	{
		LOG_FUNC_RETURN(p15card->card->ctx, SC_ERROR_OUT_OF_MEMORY);
	}
	info->id		= *id;
	info->authority		= authority;
	if (path)
		info->path = *path;

	return sc_pkcs15emu_add_object(p15card, type, label, info, NULL, obj_flags);
}