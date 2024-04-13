coolkey_logout(sc_card_t *card)
{
	/* when we add multi pin support here, how do we know which pin to logout? */
	coolkey_private_data_t * priv = COOLKEY_DATA(card);
	u8 pin_ref = 0;

	(void) coolkey_apdu_io(card, COOLKEY_CLASS, COOLKEY_INS_LOGOUT, pin_ref, 0, NULL, 0, NULL, NULL,
		priv->nonce, sizeof(priv->nonce));
	/* even if logout failed on the card, flush the nonce and clear the nonce_valid and we are effectively
	 * logged out... needing to login again to get a nonce back */
	memset(priv->nonce, 0, sizeof(priv->nonce));
	priv->nonce_valid = 0;
	return SC_SUCCESS;
}