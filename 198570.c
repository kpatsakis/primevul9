static void cac_free_private_data(cac_private_data_t *priv)
{
	free(priv->cac_id);
	free(priv->cache_buf);
	free(priv->aca_path);
	list_destroy(&priv->pki_list);
	list_destroy(&priv->general_list);
	free(priv);
	return;
}