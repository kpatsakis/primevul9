int sc_pkcs15emu_sc_hsm_decode_cvc(sc_pkcs15_card_t * p15card,
											const u8 ** buf, size_t *buflen,
											sc_cvc_t *cvc)
{
	sc_card_t *card = p15card->card;
	struct sc_asn1_entry asn1_req[C_ASN1_REQ_SIZE];
	struct sc_asn1_entry asn1_authreq[C_ASN1_AUTHREQ_SIZE];
	struct sc_asn1_entry asn1_cvc[C_ASN1_CVC_SIZE];
	struct sc_asn1_entry asn1_cvcert[C_ASN1_CVCERT_SIZE];
	struct sc_asn1_entry asn1_cvc_body[C_ASN1_CVC_BODY_SIZE];
	struct sc_asn1_entry asn1_cvc_pubkey[C_ASN1_CVC_PUBKEY_SIZE];
	unsigned int cla,tag;
	size_t taglen;
	size_t lenchr = sizeof(cvc->chr);
	size_t lencar = sizeof(cvc->car);
	size_t lenoutercar = sizeof(cvc->outer_car);
	const u8 *tbuf;
	int r;

	memset(cvc, 0, sizeof(*cvc));
	sc_copy_asn1_entry(c_asn1_req, asn1_req);
	sc_copy_asn1_entry(c_asn1_authreq, asn1_authreq);
	sc_copy_asn1_entry(c_asn1_cvc, asn1_cvc);
	sc_copy_asn1_entry(c_asn1_cvcert, asn1_cvcert);
	sc_copy_asn1_entry(c_asn1_cvc_body, asn1_cvc_body);
	sc_copy_asn1_entry(c_asn1_cvc_pubkey, asn1_cvc_pubkey);

	sc_format_asn1_entry(asn1_cvc_pubkey    , &cvc->pukoid, NULL, 0);
	sc_format_asn1_entry(asn1_cvc_pubkey + 1, &cvc->primeOrModulus, &cvc->primeOrModuluslen, 0);
	sc_format_asn1_entry(asn1_cvc_pubkey + 2, &cvc->coefficientAorExponent, &cvc->coefficientAorExponentlen, 0);
	sc_format_asn1_entry(asn1_cvc_pubkey + 3, &cvc->coefficientB, &cvc->coefficientBlen, 0);
	sc_format_asn1_entry(asn1_cvc_pubkey + 4, &cvc->basePointG, &cvc->basePointGlen, 0);
	sc_format_asn1_entry(asn1_cvc_pubkey + 5, &cvc->order, &cvc->orderlen, 0);
	sc_format_asn1_entry(asn1_cvc_pubkey + 6, &cvc->publicPoint, &cvc->publicPointlen, 0);
	sc_format_asn1_entry(asn1_cvc_pubkey + 7, &cvc->cofactor, &cvc->cofactorlen, 0);
	sc_format_asn1_entry(asn1_cvc_pubkey + 8, &cvc->modulusSize, NULL, 0);

	sc_format_asn1_entry(asn1_cvc_body    , &cvc->cpi, NULL, 0);
	sc_format_asn1_entry(asn1_cvc_body + 1, &cvc->car, &lencar, 0);
	sc_format_asn1_entry(asn1_cvc_body + 2, &asn1_cvc_pubkey, NULL, 0);
	sc_format_asn1_entry(asn1_cvc_body + 3, &cvc->chr, &lenchr, 0);

	sc_format_asn1_entry(asn1_cvcert    , &asn1_cvc_body, NULL, 0);
	sc_format_asn1_entry(asn1_cvcert + 1, &cvc->signature, &cvc->signatureLen, 0);

	sc_format_asn1_entry(asn1_cvc , &asn1_cvcert, NULL, 0);

	sc_format_asn1_entry(asn1_authreq    , &asn1_cvcert, NULL, 0);
	sc_format_asn1_entry(asn1_authreq + 1, &cvc->outer_car, &lenoutercar, 0);
	sc_format_asn1_entry(asn1_authreq + 2, &cvc->outerSignature, &cvc->outerSignatureLen, 0);

	sc_format_asn1_entry(asn1_req , &asn1_authreq, NULL, 0);

/*	sc_asn1_print_tags(*buf, *buflen); */

	tbuf = *buf;
	r = sc_asn1_read_tag(&tbuf, *buflen, &cla, &tag, &taglen);
	LOG_TEST_RET(card->ctx, r, "Could not decode card verifiable certificate");

	/*  Determine if we deal with an authenticated request, plain request or certificate */
	if ((cla == (SC_ASN1_TAG_APPLICATION|SC_ASN1_TAG_CONSTRUCTED)) && (tag == 7)) {
		r = sc_asn1_decode(card->ctx, asn1_req, *buf, *buflen, buf, buflen);
	} else {
		r = sc_asn1_decode(card->ctx, asn1_cvc, *buf, *buflen, buf, buflen);
	}

	LOG_TEST_RET(card->ctx, r, "Could not decode card verifiable certificate");

	LOG_FUNC_RETURN(card->ctx, SC_SUCCESS);
}