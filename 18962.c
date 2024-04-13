_kdc_add_KRB5SignedPath(krb5_context context,
			krb5_kdc_configuration *config,
			hdb_entry_ex *krbtgt,
			krb5_enctype enctype,
			krb5_const_principal client,
			krb5_const_principal server,
			krb5_principals principals,
			EncTicketPart *tkt)
{
    krb5_error_code ret;
    KRB5SignedPath sp;
    krb5_data data;
    krb5_crypto crypto = NULL;
    size_t size = 0;

    if (server && principals) {
	ret = add_Principals(principals, server);
	if (ret)
	    return ret;
    }

    {
	KRB5SignedPathData spd;

	spd.client = rk_UNCONST(client);
	spd.authtime = tkt->authtime;
	spd.delegated = principals;
	spd.method_data = NULL;

	ASN1_MALLOC_ENCODE(KRB5SignedPathData, data.data, data.length,
			   &spd, &size, ret);
	if (ret)
	    return ret;
	if (data.length != size)
	    krb5_abortx(context, "internal asn.1 encoder error");
    }

    {
	Key *key;
	ret = hdb_enctype2key(context, &krbtgt->entry, NULL, enctype, &key);
	if (ret == 0)
	    ret = krb5_crypto_init(context, &key->key, 0, &crypto);
	if (ret) {
	    free(data.data);
	    return ret;
	}
    }

    /*
     * Fill in KRB5SignedPath
     */

    sp.etype = enctype;
    sp.delegated = principals;
    sp.method_data = NULL;

    ret = krb5_create_checksum(context, crypto, KRB5_KU_KRB5SIGNEDPATH, 0,
			       data.data, data.length, &sp.cksum);
    krb5_crypto_destroy(context, crypto);
    free(data.data);
    if (ret)
	return ret;

    ASN1_MALLOC_ENCODE(KRB5SignedPath, data.data, data.length, &sp, &size, ret);
    free_Checksum(&sp.cksum);
    if (ret)
	return ret;
    if (data.length != size)
	krb5_abortx(context, "internal asn.1 encoder error");


    /*
     * Add IF-RELEVANT(KRB5SignedPath) to the last slot in
     * authorization data field.
     */

    ret = _kdc_tkt_add_if_relevant_ad(context, tkt,
				      KRB5_AUTHDATA_SIGNTICKET, &data);
    krb5_data_free(&data);

    return ret;
}