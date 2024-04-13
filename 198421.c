static int muscle_restore_security_env(sc_card_t *card, int se_num)
{
	muscle_private_t* priv = MUSCLE_DATA(card);
	memset(&priv->env, 0, sizeof(priv->env));
	return 0;
}