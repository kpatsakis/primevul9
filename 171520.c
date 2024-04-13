gnutls_x509_dn_get_rdn_ava(gnutls_x509_dn_t dn,
			   int irdn, int iava, gnutls_x509_ava_st * ava)
{
	ASN1_TYPE rdn, elem;
	ASN1_DATA_NODE vnode;
	long len;
	int lenlen, remlen, ret;
	char rbuf[ASN1_MAX_NAME_SIZE];
	unsigned char cls;
	const unsigned char *ptr;

	iava++;
	irdn++;			/* 0->1, 1->2 etc */

	snprintf(rbuf, sizeof(rbuf), "rdnSequence.?%d.?%d", irdn, iava);
	rdn = asn1_find_node(dn, rbuf);
	if (!rdn) {
		gnutls_assert();
		return GNUTLS_E_ASN1_ELEMENT_NOT_FOUND;
	}

	snprintf(rbuf, sizeof(rbuf), "?%d.type", iava);
	elem = asn1_find_node(rdn, rbuf);
	if (!elem) {
		gnutls_assert();
		return GNUTLS_E_ASN1_ELEMENT_NOT_FOUND;
	}

	ret = asn1_read_node_value(elem, &vnode);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		return GNUTLS_E_ASN1_ELEMENT_NOT_FOUND;
	}

	ava->oid.data = (void *) vnode.value;
	ava->oid.size = vnode.value_len;

	snprintf(rbuf, sizeof(rbuf), "?%d.value", iava);
	elem = asn1_find_node(rdn, rbuf);
	if (!elem) {
		gnutls_assert();
		return GNUTLS_E_ASN1_ELEMENT_NOT_FOUND;
	}

	ret = asn1_read_node_value(elem, &vnode);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		return GNUTLS_E_ASN1_ELEMENT_NOT_FOUND;
	}
	/* The value still has the previous tag's length bytes, plus the
	 * current value's tag and length bytes. Decode them.
	 */

	ptr = vnode.value;
	remlen = vnode.value_len;
	len = asn1_get_length_der(ptr, remlen, &lenlen);
	if (len < 0) {
		gnutls_assert();
		return GNUTLS_E_ASN1_DER_ERROR;
	}

	ptr += lenlen;
	remlen -= lenlen;
	ret =
	    asn1_get_tag_der(ptr, remlen, &cls, &lenlen, &ava->value_tag);
	if (ret) {
		gnutls_assert();
		return _gnutls_asn2err(ret);
	}

	ptr += lenlen;
	remlen -= lenlen;

	{
		signed long tmp;

		tmp = asn1_get_length_der(ptr, remlen, &lenlen);
		if (tmp < 0) {
			gnutls_assert();
			return GNUTLS_E_ASN1_DER_ERROR;
		}
		ava->value.size = tmp;
	}
	ava->value.data = (void *) (ptr + lenlen);

	return 0;
}