int gnutls_x509_ext_import_policies(const gnutls_datum_t * ext,
				 gnutls_x509_policies_t policies,
				 unsigned int flags)
{
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;
	char tmpstr[128];
	char tmpoid[MAX_OID_SIZE];
	gnutls_datum_t tmpd = { NULL, 0 };
	int ret, len;
	unsigned i, j, current = 0;

	ret = asn1_create_element
	    (_gnutls_get_pkix(), "PKIX1.certificatePolicies", &c2);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		ret = _gnutls_asn2err(ret);
		goto cleanup;
	}

	ret = _asn1_strict_der_decode(&c2, ext->data, ext->size, NULL);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		ret = _gnutls_asn2err(ret);
		goto cleanup;
	}

	for (j = 0;; j++) {
		if (j >= MAX_ENTRIES)
			break;

		memset(&policies->policy[j], 0,
		       sizeof(struct gnutls_x509_policy_st));

		/* create a string like "?1"
		 */
		snprintf(tmpstr, sizeof(tmpstr), "?%u.policyIdentifier", j + 1);
		current = j;

		ret = _gnutls_x509_read_value(c2, tmpstr, &tmpd);
		if (ret == GNUTLS_E_ASN1_ELEMENT_NOT_FOUND)
			break;

		if (ret < 0) {
			gnutls_assert();
			goto full_cleanup;
		}

		policies->policy[j].oid = (void *)tmpd.data;
		tmpd.data = NULL;

		for (i = 0; i < GNUTLS_MAX_QUALIFIERS; i++) {
			gnutls_datum_t td;

			snprintf(tmpstr, sizeof(tmpstr),
				 "?%u.policyQualifiers.?%u.policyQualifierId",
				 j + 1, i + 1);

			len = sizeof(tmpoid);
			ret = asn1_read_value(c2, tmpstr, tmpoid, &len);

			if (ret == ASN1_ELEMENT_NOT_FOUND)
				break;	/* finished */

			if (ret != ASN1_SUCCESS) {
				gnutls_assert();
				ret = _gnutls_asn2err(ret);
				goto cleanup;
			}

			if (strcmp(tmpoid, "1.3.6.1.5.5.7.2.1") == 0) {
				snprintf(tmpstr, sizeof(tmpstr),
					 "?%u.policyQualifiers.?%u.qualifier",
					 j + 1, i + 1);

				ret =
				    _gnutls_x509_read_string(c2, tmpstr, &td,
							     ASN1_ETYPE_IA5_STRING, 0);
				if (ret < 0) {
					gnutls_assert();
					goto full_cleanup;
				}

				policies->policy[j].qualifier[i].data =
				    (void *)td.data;
				policies->policy[j].qualifier[i].size = td.size;
				td.data = NULL;
				policies->policy[j].qualifier[i].type =
				    GNUTLS_X509_QUALIFIER_URI;
			} else if (strcmp(tmpoid, "1.3.6.1.5.5.7.2.2") == 0) {
				gnutls_datum_t txt = {NULL, 0};

				snprintf(tmpstr, sizeof(tmpstr),
					 "?%u.policyQualifiers.?%u.qualifier",
					 j + 1, i + 1);

				ret = _gnutls_x509_read_value(c2, tmpstr, &td);
				if (ret < 0) {
					gnutls_assert();
					goto full_cleanup;
				}

				ret =
				    decode_user_notice(td.data, td.size, &txt);
				gnutls_free(td.data);
				td.data = NULL;

				if (ret < 0) {
					gnutls_assert();
					goto full_cleanup;
				}

				policies->policy[j].qualifier[i].data =
				    (void *)txt.data;
				policies->policy[j].qualifier[i].size =
				    txt.size;
				policies->policy[j].qualifier[i].type =
				    GNUTLS_X509_QUALIFIER_NOTICE;
			} else
				policies->policy[j].qualifier[i].type =
				    GNUTLS_X509_QUALIFIER_UNKNOWN;

			policies->policy[j].qualifiers++;
		}

	}

	policies->size = j;

	ret = 0;
	goto cleanup;

 full_cleanup:
	for (j = 0; j < current; j++)
		gnutls_x509_policy_release(&policies->policy[j]);

 cleanup:
	_gnutls_free_datum(&tmpd);
	asn1_delete_structure(&c2);
	return ret;

}