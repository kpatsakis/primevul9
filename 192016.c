void rtl8xxxu_path_adda_on(struct rtl8xxxu_priv *priv, const u32 *regs,
			   bool path_a_on)
{
	u32 path_on;
	int i;

	if (priv->tx_paths == 1) {
		path_on = priv->fops->adda_1t_path_on;
		rtl8xxxu_write32(priv, regs[0], priv->fops->adda_1t_init);
	} else {
		path_on = path_a_on ? priv->fops->adda_2t_path_on_a :
			priv->fops->adda_2t_path_on_b;

		rtl8xxxu_write32(priv, regs[0], path_on);
	}

	for (i = 1 ; i < RTL8XXXU_ADDA_REGS ; i++)
		rtl8xxxu_write32(priv, regs[i], path_on);
}