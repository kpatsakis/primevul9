int sc_pkcs15emu_sc_hsm_encode_cvc(sc_pkcs15_card_t * p15card,
		sc_cvc_t *cvc,
		u8 ** buf, size_t *buflen)
{
	sc_card_t *card = p15card->card;
	struct sc_asn1_entry asn1_cvc[C_ASN1_CVC_SIZE];
	struct sc_asn1_entry asn1_cvcert[C_ASN1_CVCERT_SIZE];
	struct sc_asn1_entry asn1_cvc_body[C_ASN1_CVC_BODY_SIZE];
	struct sc_asn1_entry asn1_cvc_pubkey[C_ASN1_CVC_PUBKEY_SIZE];
	size_t lenchr;
	size_t lencar;
	int r;

	sc_copy_asn1_entry(c_asn1_cvc, asn1_cvc);
	sc_copy_asn1_entry(c_asn1_cvcert, asn1_cvcert);
	sc_copy_asn1_entry(c_asn1_cvc_body, asn1_cvc_body);
	sc_copy_asn1_entry(c_asn1_cvc_pubkey, asn1_cvc_pubkey);

	asn1_cvc_pubkey[1].flags = SC_ASN1_OPTIONAL;
	asn1_cvcert[1].flags = SC_ASN1_OPTIONAL;

	sc_format_asn1_entry(asn1_cvc_pubkey    , &cvc->pukoid, NULL, 1);
	if (cvc->primeOrModulus && (cvc->primeOrModuluslen > 0)) {
		sc_format_asn1_entry(asn1_cvc_pubkey + 1, cvc->primeOrModulus, &cvc->primeOrModuluslen, 1);
	}
	sc_format_asn1_entry(asn1_cvc_pubkey + 2, cvc->coefficientAorExponent, &cvc->coefficientAorExponentlen, 1);
	if (cvc->coefficientB && (cvc->coefficientBlen > 0)) {
		sc_format_asn1_entry(asn1_cvc_pubkey + 3, cvc->coefficientB, &cvc->coefficientBlen, 1);
		sc_format_asn1_entry(asn1_cvc_pubkey + 4, cvc->basePointG, &cvc->basePointGlen, 1);
		sc_format_asn1_entry(asn1_cvc_pubkey + 5, cvc->order, &cvc->orderlen, 1);
		if (cvc->publicPoint && (cvc->publicPointlen > 0)) {
			sc_format_asn1_entry(asn1_cvc_pubkey + 6, cvc->publicPoint, &cvc->publicPointlen, 1);
		}
		sc_format_asn1_entry(asn1_cvc_pubkey + 7, cvc->cofactor, &cvc->cofactorlen, 1);
	}
	if (cvc->modulusSize > 0) {
		sc_format_asn1_entry(asn1_cvc_pubkey + 8, &cvc->modulusSize, NULL, 1);
	}

	sc_format_asn1_entry(asn1_cvc_body    , &cvc->cpi, NULL, 1);
	lencar = strnlen(cvc->car, sizeof cvc->car);
	sc_format_asn1_entry(asn1_cvc_body + 1, &cvc->car, &lencar, 1);
	sc_format_asn1_entry(asn1_cvc_body + 2, &asn1_cvc_pubkey, NULL, 1);
	lenchr = strnlen(cvc->chr, sizeof cvc->chr);
	sc_format_asn1_entry(asn1_cvc_body + 3, &cvc->chr, &lenchr, 1);

	sc_format_asn1_entry(asn1_cvcert    , &asn1_cvc_body, NULL, 1);
	if (cvc->signature && (cvc->signatureLen > 0)) {
		sc_format_asn1_entry(asn1_cvcert + 1, cvc->signature, &cvc->signatureLen, 1);
	}

	sc_format_asn1_entry(asn1_cvc , &asn1_cvcert, NULL, 1);

	r = sc_asn1_encode(card->ctx, asn1_cvc, buf, buflen);
	LOG_TEST_RET(card->ctx, r, "Could not encode card verifiable certificate");

	LOG_FUNC_RETURN(card->ctx, SC_SUCCESS);
}