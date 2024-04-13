coolkey_fill_object(sc_card_t *card, sc_cardctl_coolkey_object_t *obj)
{
	int r;
	size_t buf_len = obj->length;
	u8 *new_obj_data = NULL;
	sc_cardctl_coolkey_object_t *obj_entry;
	coolkey_private_data_t * priv = COOLKEY_DATA(card);

	if (obj->data != NULL) {
		return SC_SUCCESS;
	}
	new_obj_data = malloc(buf_len);
	if (new_obj_data == NULL) {
		return SC_ERROR_OUT_OF_MEMORY;
	}
	r = coolkey_read_object(card, obj->id, 0, new_obj_data, buf_len,
				priv->nonce, sizeof(priv->nonce));
	if (r != (int)buf_len) {
		free(new_obj_data);
		return SC_ERROR_CORRUPTED_DATA;
	}
	obj_entry = coolkey_find_object_by_id(&priv->objects_list, obj->id);
	if (obj_entry == NULL) {
		free(new_obj_data);
		return SC_ERROR_INTERNAL; /* shouldn't happen */
	}
	if (obj_entry->data != NULL) {
		free(new_obj_data);
		return SC_ERROR_INTERNAL; /* shouldn't happen */
	}
	/* Make sure we will not go over the allocated limits in the other
	 * objects if they somehow got different lengths in matching objects */
	if (obj_entry->length != obj->length) {
		free(new_obj_data);
		return SC_ERROR_INTERNAL; /* shouldn't happen */
	}
	obj_entry->data = new_obj_data;
	obj->data = new_obj_data;
	return SC_SUCCESS;
}