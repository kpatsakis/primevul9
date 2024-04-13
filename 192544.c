static void coolkey_free_private_data(coolkey_private_data_t *priv)
{
	list_t *l = &priv->objects_list;
	sc_cardctl_coolkey_object_t *o;

	/* Clean up the allocated memory in the items */
	list_iterator_start(l);
	while (list_iterator_hasnext(l)) {
		o = (sc_cardctl_coolkey_object_t *)list_iterator_next(l);
		free(o->data);
		o->data = NULL;
	}
	list_iterator_stop(l);

	list_destroy(&priv->objects_list);
	if (priv->token_name) {
		free(priv->token_name);
	}
	free(priv);
	return;
}