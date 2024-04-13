char* x509_proxy_email( globus_gsi_cred_handle_t handle )
{
	X509_NAME *email_orig = NULL;
        STACK_OF(X509) *cert_chain = NULL;
	GENERAL_NAME *gen;
	GENERAL_NAMES *gens;
        X509 *cert = NULL;
	char *email = NULL, *email2 = NULL;
	int i, j;

	if ( activate_globus_gsi() != 0 ) {
		return NULL;
	}

	if ((*globus_gsi_cred_get_cert_chain_ptr)(handle, &cert_chain)) {
		cert = NULL;
		set_error_string( "unable to find certificate in proxy" );
		goto cleanup;
	}

	for(i = 0; i < sk_X509_num(cert_chain) && email == NULL; ++i) {
		if((cert = sk_X509_value(cert_chain, i)) == NULL) {
			continue;
		}
		if ((email_orig = (X509_NAME *)X509_get_ext_d2i(cert, NID_pkcs9_emailAddress, 0, 0)) != NULL) {
			if ((email2 = X509_NAME_oneline(email_orig, NULL, 0)) == NULL) {
				continue;
			} else {
				// Return something that we can free().
				email = strdup(email2);
				OPENSSL_free(email2);
				break;
			}
		}
		gens = (GENERAL_NAMES *)X509_get_ext_d2i(cert, NID_subject_alt_name, 0, 0);
		if (gens) {
			for (j = 0; j < sk_GENERAL_NAME_num(gens); ++j) {
				if ((gen = sk_GENERAL_NAME_value(gens, j)) == NULL) {
					continue;
				}
				if (gen->type != GEN_EMAIL) {
					continue;
				}
				ASN1_IA5STRING *email_ia5 = gen->d.ia5;
				// Sanity checks.
				if (email_ia5->type != V_ASN1_IA5STRING) goto cleanup;
				if (!email_ia5->data || !email_ia5->length) goto cleanup;
				email2 = BUF_strdup((char *)email_ia5->data);
				// We want to return something we can free(), so make another copy.
				if (email2) {
					email = strdup(email2);
					OPENSSL_free(email2);
				}
				break;
			}
			sk_GENERAL_NAME_pop_free(gens, GENERAL_NAME_free);
		}
	}

	if (email == NULL) {
		set_error_string( "unable to extract email" );
		goto cleanup;
	}

 cleanup:
	if (cert_chain) {
		sk_X509_pop_free(cert_chain, X509_free);
	}

	if (email_orig) {
		X509_NAME_free(email_orig);
	}

	return email;
}