sc_pkcs15emu_add_object(sc_pkcs15_card_t *p15card, int type,
		const char *label, void *data,
		const sc_pkcs15_id_t *auth_id, int obj_flags)
{
	sc_pkcs15_object_t *obj;
	int		df_type;

	obj = calloc(1, sizeof(*obj));

	obj->type  = type;
	obj->data  = data;

	if (label)
		strncpy(obj->label, label, sizeof(obj->label)-1);

	obj->flags = obj_flags;
	if (auth_id)
		obj->auth_id = *auth_id;

	switch (type & SC_PKCS15_TYPE_CLASS_MASK) {
	case SC_PKCS15_TYPE_AUTH:
		df_type = SC_PKCS15_AODF;
		break;
	case SC_PKCS15_TYPE_PRKEY:
		df_type = SC_PKCS15_PRKDF;
		break;
	case SC_PKCS15_TYPE_PUBKEY:
		df_type = SC_PKCS15_PUKDF;
		break;
	case SC_PKCS15_TYPE_CERT:
		df_type = SC_PKCS15_CDF;
		break;
	default:
		sc_log(p15card->card->ctx, "Unknown PKCS15 object type %d", type);
		free(obj);
		return SC_ERROR_INVALID_ARGUMENTS;
	}

	obj->df = sc_pkcs15emu_get_df(p15card, df_type);
	sc_pkcs15_add_object(p15card, obj);

	return 0;
}