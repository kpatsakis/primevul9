static int cac_compute_signature(sc_card_t *card,
					const u8 * data, size_t datalen,
					u8 * out, size_t outlen)
{
	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_VERBOSE, cac_rsa_op(card, data, datalen, out, outlen));
}