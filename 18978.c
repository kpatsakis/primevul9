check_KRB5SignedPath(krb5_context context,
		     krb5_kdc_configuration *config,
		     hdb_entry_ex *krbtgt,
		     krb5_principal cp,
		     EncTicketPart *tkt,
		     krb5_principals *delegated,
		     int *signedpath)
{
    krb5_error_code ret;
    krb5_data data;
    krb5_crypto crypto = NULL;

    if (delegated)
	*delegated = NULL;

    ret = find_KRB5SignedPath(context, tkt->authorization_data, &data);
    if (ret == 0) {
	KRB5SignedPathData spd;
	KRB5SignedPath sp;
	size_t size = 0;

	ret = decode_KRB5SignedPath(data.data, data.length, &sp, NULL);
	krb5_data_free(&data);
	if (ret)
	    return ret;

	spd.client = cp;
	spd.authtime = tkt->authtime;
	spd.delegated = sp.delegated;
	spd.method_data = sp.method_data;

	ASN1_MALLOC_ENCODE(KRB5SignedPathData, data.data, data.length,
			   &spd, &size, ret);
	if (ret) {
	    free_KRB5SignedPath(&sp);
	    return ret;
	}
	if (data.length != size)
	    krb5_abortx(context, "internal asn.1 encoder error");

	{
	    Key *key;
	    ret = hdb_enctype2key(context, &krbtgt->entry, NULL, /* XXX use correct kvno! */
				  sp.etype, &key);
	    if (ret == 0)
		ret = krb5_crypto_init(context, &key->key, 0, &crypto);
	    if (ret) {
		free(data.data);
		free_KRB5SignedPath(&sp);
		return ret;
	    }
	}
	ret = krb5_verify_checksum(context, crypto, KRB5_KU_KRB5SIGNEDPATH,
				   data.data, data.length,
				   &sp.cksum);
	krb5_crypto_destroy(context, crypto);
	free(data.data);
	if (ret) {
	    free_KRB5SignedPath(&sp);
	    kdc_log(context, config, 4,
		    "KRB5SignedPath not signed correctly, not marking as signed");
	    return 0;
	}

	if (delegated && sp.delegated) {

	    *delegated = malloc(sizeof(*sp.delegated));
	    if (*delegated == NULL) {
		free_KRB5SignedPath(&sp);
		return ENOMEM;
	    }

	    ret = copy_Principals(*delegated, sp.delegated);
	    if (ret) {
		free_KRB5SignedPath(&sp);
		free(*delegated);
		*delegated = NULL;
		return ret;
	    }
	}
	free_KRB5SignedPath(&sp);

	*signedpath = 1;
    }

    return 0;
}