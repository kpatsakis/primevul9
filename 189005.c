static void i40e_clear_rss_config_user(struct i40e_vsi *vsi)
{
	if (!vsi)
		return;

	kfree(vsi->rss_hkey_user);
	vsi->rss_hkey_user = NULL;

	kfree(vsi->rss_lut_user);
	vsi->rss_lut_user = NULL;
}