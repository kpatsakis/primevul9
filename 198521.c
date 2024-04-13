sc_pkcs15emu_add_pin(sc_pkcs15_card_t *p15card,
                const sc_pkcs15_id_t *id, const char *label,
                const sc_path_t *path, int ref, int type,
                unsigned int min_length,
                unsigned int max_length,
                int flags, int tries_left, const char pad_char, int obj_flags)
{
	sc_pkcs15_auth_info_t *info;

	info = calloc(1, sizeof(*info));
	if (!info)
		LOG_FUNC_RETURN(p15card->card->ctx, SC_ERROR_OUT_OF_MEMORY);

	info->auth_type = SC_PKCS15_PIN_AUTH_TYPE_PIN;
	info->auth_method = SC_AC_CHV;
	info->auth_id           = *id;
	info->attrs.pin.min_length        = min_length;
	info->attrs.pin.max_length        = max_length;
	info->attrs.pin.stored_length     = max_length;
	info->attrs.pin.type              = type;
	info->attrs.pin.reference         = ref;
	info->attrs.pin.flags             = flags;
	info->attrs.pin.pad_char          = pad_char;
	info->tries_left        = tries_left;
	info->logged_in = SC_PIN_STATE_UNKNOWN;

	if (path)
		info->path = *path;

	return sc_pkcs15emu_add_object(p15card, SC_PKCS15_TYPE_AUTH_PIN, label, info, NULL, obj_flags);
}