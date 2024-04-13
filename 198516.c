static int cac_find_and_initialize(sc_card_t *card, int initialize)
{
	int r, index;
	cac_private_data_t *priv = NULL;

	/* already initialized? */
	if (card->drv_data) {
		return SC_SUCCESS;
	}

	/* is this a CAC-2 specified in NIST Interagency Report 6887 -
	 * "Government Smart Card Interoperability Specification v2.1 July 2003" */
	r = cac_select_CCC(card);
	if (r == SC_SUCCESS) {
		sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE, "CCC found, is CAC-2");
		if (!initialize) /* match card only */
			return r;

		priv = cac_new_private_data();
		if (!priv)
			return SC_ERROR_OUT_OF_MEMORY;
		r = cac_process_CCC(card, priv);
		if (r == SC_SUCCESS) {
			card->type = SC_CARD_TYPE_CAC_II;
			card->drv_data = priv;
			return r;
		}
	}

	/* Even some ALT tokens can be missing CCC so we should try with ACA */
	r = cac_select_ACA(card);
	if (r == SC_SUCCESS) {
		sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE, "ACA found, is CAC-2 without CCC");
		if (!initialize) /* match card only */
			return r;

		if (!priv) {
			priv = cac_new_private_data();
			if (!priv)
				return SC_ERROR_OUT_OF_MEMORY;
		}
		r = cac_process_ACA(card, priv);
		if (r == SC_SUCCESS) {
			card->type = SC_CARD_TYPE_CAC_II;
			card->drv_data = priv;
			return r;
		}
	}

	/* is this a CAC Alt token without any accompanying structures */
	r = cac_find_first_pki_applet(card, &index);
	if (r == SC_SUCCESS) {
		sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE, "PKI applet found, is bare CAC Alt");
		if (!initialize) /* match card only */
			return r;

		if (!priv) {
			priv = cac_new_private_data();
			if (!priv)
				return SC_ERROR_OUT_OF_MEMORY;
		}
		card->drv_data = priv; /* needed for the read_binary() */
		r = cac_populate_cac_alt(card, index, priv);
		if (r == SC_SUCCESS) {
			card->type = SC_CARD_TYPE_CAC_II;
			return r;
		}
		card->drv_data = NULL; /* reset on failure */
	}
	if (priv) {
		cac_free_private_data(priv);
	}
	return r;
}