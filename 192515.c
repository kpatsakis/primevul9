static coolkey_private_data_t *coolkey_new_private_data(void)
{
	coolkey_private_data_t *priv;

	/* allocate priv and zero all the fields */
	priv = calloc(1, sizeof(coolkey_private_data_t));
	if (!priv)
		return NULL;

	/* set other fields as appropriate */
	priv->key_id = COOLKEY_INVALID_KEY;
	if (list_init(&priv->objects_list) != 0 ||
	    list_attributes_comparator(&priv->objects_list, coolkey_compare_id) != 0 ||
	    list_attributes_copy(&priv->objects_list, coolkey_list_meter, 1) != 0) {
		coolkey_free_private_data(priv);
		return NULL;
	}

	return priv;
}