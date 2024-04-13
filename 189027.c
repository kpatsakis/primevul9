static void free_firmware(struct xc2028_data *priv)
{
	int i;
	tuner_dbg("%s called\n", __func__);

	if (!priv->firm)
		return;

	for (i = 0; i < priv->firm_size; i++)
		kfree(priv->firm[i].ptr);

	kfree(priv->firm);

	priv->firm = NULL;
	priv->firm_size = 0;
	priv->state = XC2028_NO_FIRMWARE;

	memset(&priv->cur_fw, 0, sizeof(priv->cur_fw));
}