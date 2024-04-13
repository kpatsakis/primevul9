static int muscle_finish(sc_card_t *card)
{
	muscle_private_t *priv = MUSCLE_DATA(card);
	mscfs_free(priv->fs);
	free(priv);
	return 0;
}