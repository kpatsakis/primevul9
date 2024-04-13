int sc_pkcs15emu_gemsafeV1_init_ex( sc_pkcs15_card_t *p15card,
			struct sc_aid *aid,
			sc_pkcs15emu_opt_t *opts)
{
	if (opts && opts->flags & SC_PKCS15EMU_FLAGS_NO_CHECK)
		return sc_pkcs15emu_gemsafeV1_init(p15card);
	else {
		int r = gemsafe_detect_card(p15card);
		if (r)
			return SC_ERROR_WRONG_CARD;
		return sc_pkcs15emu_gemsafeV1_init(p15card);
	}
}