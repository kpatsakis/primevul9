epass2003_write_key(struct sc_card *card, sc_epass2003_wkey_data * data)
{
	LOG_FUNC_CALLED(card->ctx);

	if (data->type & SC_EPASS2003_KEY) {
		if (data->type == SC_EPASS2003_KEY_RSA)
			return internal_write_rsa_key(card, data->key_data.es_key.fid,
						      data->key_data.es_key.rsa);
		else
			LOG_FUNC_RETURN(card->ctx, SC_ERROR_NOT_SUPPORTED);
	} else if (data->type & SC_EPASS2003_SECRET) {
		if (data->type == SC_EPASS2003_SECRET_PRE)
			return internal_install_pre(card);
		else if (data->type == SC_EPASS2003_SECRET_PIN)
			return internal_install_pin(card, data);
		else
			LOG_FUNC_RETURN(card->ctx, SC_ERROR_NOT_SUPPORTED);
	}
	else {
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_NOT_SUPPORTED);
	}

	LOG_FUNC_RETURN(card->ctx, SC_SUCCESS);
}