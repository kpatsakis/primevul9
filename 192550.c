static int coolkey_compute_crypt(sc_card_t *card,
					const u8 * data, size_t datalen,
					u8 * out, size_t outlen)
{
	coolkey_private_data_t * priv = COOLKEY_DATA(card);
	int r;
	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	switch (priv->algorithm) {
	case SC_ALGORITHM_RSA:
		r = coolkey_rsa_op(card, data, datalen, out, outlen);
		break;
	case SC_ALGORITHM_EC:
		r = coolkey_ecc_op(card, data, datalen, out, outlen);
		break;
	default:
		r = SC_ERROR_NO_CARD_SUPPORT;
		break;
	}

	SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_VERBOSE, r);
}