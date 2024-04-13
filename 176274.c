check_server_referral(krb5_context context,
		      krb5_kdc_rep *rep,
		      unsigned flags,
		      krb5_const_principal requested,
		      krb5_const_principal returned,
		      krb5_keyblock * key)
{
    krb5_error_code ret;
    PA_ServerReferralData ref;
    krb5_crypto session;
    EncryptedData ed;
    size_t len;
    krb5_data data;
    PA_DATA *pa;
    int i = 0, cmp;

    if (rep->kdc_rep.padata == NULL)
	goto noreferral;

    pa = krb5_find_padata(rep->kdc_rep.padata->val,
			  rep->kdc_rep.padata->len,
			  KRB5_PADATA_SERVER_REFERRAL, &i);
    if (pa == NULL)
	goto noreferral;

    memset(&ed, 0, sizeof(ed));
    memset(&ref, 0, sizeof(ref));

    ret = decode_EncryptedData(pa->padata_value.data,
			       pa->padata_value.length,
			       &ed, &len);
    if (ret)
	return ret;
    if (len != pa->padata_value.length) {
	free_EncryptedData(&ed);
	krb5_set_error_message(context, KRB5KRB_AP_ERR_MODIFIED,
			       N_("Referral EncryptedData wrong for realm %s",
				  "realm"), requested->realm);
	return KRB5KRB_AP_ERR_MODIFIED;
    }

    ret = krb5_crypto_init(context, key, 0, &session);
    if (ret) {
	free_EncryptedData(&ed);
	return ret;
    }

    ret = krb5_decrypt_EncryptedData(context, session,
				     KRB5_KU_PA_SERVER_REFERRAL,
				     &ed, &data);
    free_EncryptedData(&ed);
    krb5_crypto_destroy(context, session);
    if (ret)
	return ret;

    ret = decode_PA_ServerReferralData(data.data, data.length, &ref, &len);
    if (ret) {
	krb5_data_free(&data);
	return ret;
    }
    krb5_data_free(&data);

    if (strcmp(requested->realm, returned->realm) != 0) {
	free_PA_ServerReferralData(&ref);
	krb5_set_error_message(context, KRB5KRB_AP_ERR_MODIFIED,
			       N_("server ref realm mismatch, "
				  "requested realm %s got back %s", ""),
			       requested->realm, returned->realm);
	return KRB5KRB_AP_ERR_MODIFIED;
    }

    if (krb5_principal_is_krbtgt(context, returned)) {
	const char *realm = returned->name.name_string.val[1];

	if (ref.referred_realm == NULL
	    || strcmp(*ref.referred_realm, realm) != 0)
	{
	    free_PA_ServerReferralData(&ref);
	    krb5_set_error_message(context, KRB5KRB_AP_ERR_MODIFIED,
				   N_("tgt returned with wrong ref", ""));
	    return KRB5KRB_AP_ERR_MODIFIED;
	}
    } else if (krb5_principal_compare(context, returned, requested) == 0) {
	free_PA_ServerReferralData(&ref);
	krb5_set_error_message(context, KRB5KRB_AP_ERR_MODIFIED,
			       N_("req princ no same as returned", ""));
	return KRB5KRB_AP_ERR_MODIFIED;
    }

    if (ref.requested_principal_name) {
	cmp = _krb5_principal_compare_PrincipalName(context,
						    requested,
						    ref.requested_principal_name);
	if (!cmp) {
	    free_PA_ServerReferralData(&ref);
	    krb5_set_error_message(context, KRB5KRB_AP_ERR_MODIFIED,
				   N_("referred principal not same "
				      "as requested", ""));
	    return KRB5KRB_AP_ERR_MODIFIED;
	}
    } else if (flags & EXTRACT_TICKET_AS_REQ) {
	free_PA_ServerReferralData(&ref);
	krb5_set_error_message(context, KRB5KRB_AP_ERR_MODIFIED,
			       N_("Requested principal missing on AS-REQ", ""));
	return KRB5KRB_AP_ERR_MODIFIED;
    }

    free_PA_ServerReferralData(&ref);

    return ret;
noreferral:
    /*
     * Expect excact match or that we got a krbtgt
     */
    if (krb5_principal_compare(context, requested, returned) != TRUE &&
	(krb5_realm_compare(context, requested, returned) != TRUE &&
	 krb5_principal_is_krbtgt(context, returned) != TRUE))
    {
	krb5_set_error_message(context, KRB5KRB_AP_ERR_MODIFIED,
			       N_("Not same server principal returned "
				  "as requested", ""));
	return KRB5KRB_AP_ERR_MODIFIED;
    }
    return 0;
}