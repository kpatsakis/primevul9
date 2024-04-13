static int cac_is_cert(cac_private_data_t * priv, const sc_path_t *in_path)
{
	cac_object_t test_obj;
	test_obj.path = *in_path;
	test_obj.path.index = 0;
	test_obj.path.count = 0;

	return (list_contains(&priv->pki_list, &test_obj) != 0);
}