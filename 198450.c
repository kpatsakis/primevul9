int sc_pkcs15emu_sc_hsm_init_ex(sc_pkcs15_card_t *p15card,
				struct sc_aid *aid,
				sc_pkcs15emu_opt_t *opts)
{
	if (opts && (opts->flags & SC_PKCS15EMU_FLAGS_NO_CHECK)) {
		return sc_pkcs15emu_sc_hsm_init(p15card);
	} else {
		if (p15card->card->type != SC_CARD_TYPE_SC_HSM
				&& p15card->card->type != SC_CARD_TYPE_SC_HSM_SOC
				&& p15card->card->type != SC_CARD_TYPE_SC_HSM_GOID) {
			return SC_ERROR_WRONG_CARD;
		}
		return sc_pkcs15emu_sc_hsm_init(p15card);
	}
}