static cac_private_data_t *cac_new_private_data(void)
{
	cac_private_data_t *priv;
	priv = calloc(1, sizeof(cac_private_data_t));
	if (!priv)
		return NULL;
	list_init(&priv->pki_list);
	list_attributes_comparator(&priv->pki_list, cac_list_compare_path);
	list_attributes_copy(&priv->pki_list, cac_list_meter, 1);
	list_init(&priv->general_list);
	list_attributes_comparator(&priv->general_list, cac_list_compare_path);
	list_attributes_copy(&priv->general_list, cac_list_meter, 1);
	/* set other fields as appropriate */

	return priv;
}