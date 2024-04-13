static int coolkey_select_file(sc_card_t *card, const sc_path_t *in_path, sc_file_t **file_out)
{
	int r;
	struct sc_file *file = NULL;
	coolkey_private_data_t * priv = COOLKEY_DATA(card);
	unsigned long object_id;

	assert(card != NULL && in_path != NULL);

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	if (in_path->len != 4) {
		return SC_ERROR_OBJECT_NOT_FOUND;
	}
	r = coolkey_select_applet(card);
	if (r != SC_SUCCESS) {
		return r;
	}
	object_id = bebytes2ulong(in_path->value);
	priv->obj = coolkey_find_object_by_id(&priv->objects_list, object_id);
	if (priv->obj == NULL) {
		return SC_ERROR_OBJECT_NOT_FOUND;
	}

	priv->key_id = COOLKEY_INVALID_KEY;
	if (coolkey_class(object_id) == COOLKEY_KEY_CLASS) {
		priv->key_id = coolkey_get_key_id(object_id);
	}
	if (file_out) {
		file = sc_file_new();
		if (file == NULL)
			LOG_FUNC_RETURN(card->ctx, SC_ERROR_OUT_OF_MEMORY);
		file->path = *in_path;
		/* this could be like the FCI */
		file->type =  SC_PATH_TYPE_FILE_ID;
		file->shareable = 0;
		file->ef_structure = 0;
		file->size = priv->obj->length;
		*file_out = file;
	}

	return SC_SUCCESS;
}