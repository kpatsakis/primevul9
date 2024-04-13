internal_write_rsa_key(struct sc_card *card, unsigned short fid, struct sc_pkcs15_prkey_rsa *rsa)
{
	int r;

	LOG_FUNC_CALLED(card->ctx);

	r = internal_write_rsa_key_factor(card, fid, 0x02, rsa->modulus);
	LOG_TEST_RET(card->ctx, r, "write n failed");
	r = internal_write_rsa_key_factor(card, fid, 0x03, rsa->d);
	LOG_TEST_RET(card->ctx, r, "write d failed");

	LOG_FUNC_RETURN(card->ctx, SC_SUCCESS);
}