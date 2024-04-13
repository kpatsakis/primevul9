build_server_referral(krb5_context context,
		      krb5_kdc_configuration *config,
		      krb5_crypto session,
		      krb5_const_realm referred_realm,
		      const PrincipalName *true_principal_name,
		      const PrincipalName *requested_principal,
		      krb5_data *outdata)
{
    PA_ServerReferralData ref;
    krb5_error_code ret;
    EncryptedData ed;
    krb5_data data;
    size_t size = 0;

    memset(&ref, 0, sizeof(ref));

    if (referred_realm) {
	ALLOC(ref.referred_realm);
	if (ref.referred_realm == NULL)
	    goto eout;
	*ref.referred_realm = strdup(referred_realm);
	if (*ref.referred_realm == NULL)
	    goto eout;
    }
    if (true_principal_name) {
	ALLOC(ref.true_principal_name);
	if (ref.true_principal_name == NULL)
	    goto eout;
	ret = copy_PrincipalName(true_principal_name, ref.true_principal_name);
	if (ret)
	    goto eout;
    }
    if (requested_principal) {
	ALLOC(ref.requested_principal_name);
	if (ref.requested_principal_name == NULL)
	    goto eout;
	ret = copy_PrincipalName(requested_principal,
				 ref.requested_principal_name);
	if (ret)
	    goto eout;
    }

    ASN1_MALLOC_ENCODE(PA_ServerReferralData,
		       data.data, data.length,
		       &ref, &size, ret);
    free_PA_ServerReferralData(&ref);
    if (ret)
	return ret;
    if (data.length != size)
	krb5_abortx(context, "internal asn.1 encoder error");

    ret = krb5_encrypt_EncryptedData(context, session,
				     KRB5_KU_PA_SERVER_REFERRAL,
				     data.data, data.length,
				     0 /* kvno */, &ed);
    free(data.data);
    if (ret)
	return ret;

    ASN1_MALLOC_ENCODE(EncryptedData,
		       outdata->data, outdata->length,
		       &ed, &size, ret);
    free_EncryptedData(&ed);
    if (ret)
	return ret;
    if (outdata->length != size)
	krb5_abortx(context, "internal asn.1 encoder error");

    return 0;
eout:
    free_PA_ServerReferralData(&ref);
    krb5_set_error_message(context, ENOMEM, "malloc: out of memory");
    return ENOMEM;
}