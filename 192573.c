static int coolkey_get_token_info(sc_card_t *card, sc_pkcs15_tokeninfo_t * token_info)
{
	coolkey_private_data_t * priv = COOLKEY_DATA(card);
	char *label = NULL;
	char *manufacturer_id = NULL;
	char *serial_number = NULL;

	LOG_FUNC_CALLED(card->ctx);
	label = strdup((char *)priv->token_name);
	manufacturer_id = coolkey_get_manufacturer(&priv->cuid);
	serial_number = coolkey_cuid_to_string(&priv->cuid);

	if (label && manufacturer_id && serial_number) {
		token_info->label = label;
		token_info->manufacturer_id = manufacturer_id;
		token_info->serial_number = serial_number;
		return SC_SUCCESS;
	}
	free(label);
	free(manufacturer_id);
	free(serial_number);
	return SC_ERROR_OUT_OF_MEMORY;
}