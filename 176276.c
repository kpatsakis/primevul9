decrypt_tkt (krb5_context context,
	     krb5_keyblock *key,
	     krb5_key_usage usage,
	     krb5_const_pointer decrypt_arg,
	     krb5_kdc_rep *dec_rep)
{
    krb5_error_code ret;
    krb5_data data;
    size_t size;
    krb5_crypto crypto;

    ret = krb5_crypto_init(context, key, 0, &crypto);
    if (ret)
	return ret;

    ret = krb5_decrypt_EncryptedData (context,
				      crypto,
				      usage,
				      &dec_rep->kdc_rep.enc_part,
				      &data);
    krb5_crypto_destroy(context, crypto);

    if (ret)
	return ret;

    ret = decode_EncASRepPart(data.data,
			      data.length,
			      &dec_rep->enc_part,
			      &size);
    if (ret)
	ret = decode_EncTGSRepPart(data.data,
				   data.length,
				   &dec_rep->enc_part,
				   &size);
    krb5_data_free (&data);
    if (ret) {
        krb5_set_error_message(context, ret,
			       N_("Failed to decode encpart in ticket", ""));
	return ret;
    }
    return 0;
}