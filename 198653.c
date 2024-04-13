internal_install_pre(struct sc_card *card)
{
	int r;
	/* init key for enc */
	r = install_secret_key(card, 0x01, 0x00,
			       EPASS2003_AC_MAC_NOLESS | EPASS2003_AC_EVERYONE,
			       EPASS2003_AC_MAC_NOLESS | EPASS2003_AC_EVERYONE,
			       0, g_init_key_enc, 16);
	LOG_TEST_RET(card->ctx, r, "Install init key failed");

	/* init key for mac */
	r = install_secret_key(card, 0x02, 0x00,
			       EPASS2003_AC_MAC_NOLESS | EPASS2003_AC_EVERYONE,
			       EPASS2003_AC_MAC_NOLESS | EPASS2003_AC_EVERYONE,
			       0, g_init_key_mac, 16);
	LOG_TEST_RET(card->ctx, r, "Install init key failed");

	return r;
}