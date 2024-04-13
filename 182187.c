void sc_pkcs15_free_prkey_info(sc_pkcs15_prkey_info_t *key)
{
	if (key->subject.value)
		free(key->subject.value);

	sc_pkcs15_free_key_params(&key->params);

	sc_aux_data_free(&key->aux_data);

	free(key);
}