tgs_check_authenticator(krb5_context context,
			krb5_kdc_configuration *config,
	                krb5_auth_context ac,
			KDC_REQ_BODY *b,
			const char **e_text,
			krb5_keyblock *key)
{
    krb5_authenticator auth;
    size_t len = 0;
    unsigned char *buf;
    size_t buf_size;
    krb5_error_code ret;
    krb5_crypto crypto;

    krb5_auth_con_getauthenticator(context, ac, &auth);
    if(auth->cksum == NULL){
	kdc_log(context, config, 4, "No authenticator in request");
	ret = KRB5KRB_AP_ERR_INAPP_CKSUM;
	goto out;
    }
    /*
     * according to RFC1510 it doesn't need to be keyed,
     * but according to the latest draft it needs to.
     */
    if (
#if 0
!krb5_checksum_is_keyed(context, auth->cksum->cksumtype)
	||
#endif
 !krb5_checksum_is_collision_proof(context, auth->cksum->cksumtype)) {
	kdc_log(context, config, 4, "Bad checksum type in authenticator: %d",
		auth->cksum->cksumtype);
	ret =  KRB5KRB_AP_ERR_INAPP_CKSUM;
	goto out;
    }

    /* XXX should not re-encode this */
    ASN1_MALLOC_ENCODE(KDC_REQ_BODY, buf, buf_size, b, &len, ret);
    if(ret){
	const char *msg = krb5_get_error_message(context, ret);
	kdc_log(context, config, 4, "Failed to encode KDC-REQ-BODY: %s", msg);
	krb5_free_error_message(context, msg);
	goto out;
    }
    if(buf_size != len) {
	free(buf);
	kdc_log(context, config, 4, "Internal error in ASN.1 encoder");
	*e_text = "KDC internal error";
	ret = KRB5KRB_ERR_GENERIC;
	goto out;
    }
    ret = krb5_crypto_init(context, key, 0, &crypto);
    if (ret) {
	const char *msg = krb5_get_error_message(context, ret);
	free(buf);
	kdc_log(context, config, 4, "krb5_crypto_init failed: %s", msg);
	krb5_free_error_message(context, msg);
	goto out;
    }
    ret = krb5_verify_checksum(context,
			       crypto,
			       KRB5_KU_TGS_REQ_AUTH_CKSUM,
			       buf,
			       len,
			       auth->cksum);
    free(buf);
    krb5_crypto_destroy(context, crypto);
    if(ret){
	const char *msg = krb5_get_error_message(context, ret);
	kdc_log(context, config, 4,
		"Failed to verify authenticator checksum: %s", msg);
	krb5_free_error_message(context, msg);
    }
out:
    free_Authenticator(auth);
    free(auth);
    return ret;
}