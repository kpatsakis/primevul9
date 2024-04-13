ec_return(krb5_context context, krb5_pa_data *padata, krb5_data *req_pkt,
          krb5_kdc_req *request, krb5_kdc_rep *reply,
          krb5_keyblock *encrypting_key, krb5_pa_data **send_pa,
          krb5_kdcpreauth_callbacks cb, krb5_kdcpreauth_rock rock,
          krb5_kdcpreauth_moddata moddata, krb5_kdcpreauth_modreq modreq)
{
    krb5_error_code retval = 0;
    krb5_keyblock *challenge_key = (krb5_keyblock *)modreq;
    krb5_pa_enc_ts ts;
    krb5_data *plain = NULL;
    krb5_enc_data enc;
    krb5_data *encoded = NULL;
    krb5_pa_data *pa = NULL;

    if (challenge_key == NULL)
        return 0;
    enc.ciphertext.data = NULL; /* In case of error pass through */

    retval = krb5_us_timeofday(context, &ts.patimestamp, &ts.pausec);
    if (retval == 0)
        retval = encode_krb5_pa_enc_ts(&ts, &plain);
    if (retval == 0)
        retval = krb5_encrypt_helper(context, challenge_key,
                                     KRB5_KEYUSAGE_ENC_CHALLENGE_KDC,
                                     plain, &enc);
    if (retval == 0)
        retval = encode_krb5_enc_data(&enc, &encoded);
    if (retval == 0) {
        pa = calloc(1, sizeof(krb5_pa_data));
        if (pa == NULL)
            retval = ENOMEM;
    }
    if (retval == 0) {
        pa->pa_type = KRB5_PADATA_ENCRYPTED_CHALLENGE;
        pa->contents = (unsigned char *) encoded->data;
        pa->length = encoded->length;
        encoded->data = NULL;
        *send_pa = pa;
        pa = NULL;
    }
    if (challenge_key)
        krb5_free_keyblock(context, challenge_key);
    if (encoded)
        krb5_free_data(context, encoded);
    if (plain)
        krb5_free_data(context, plain);
    if (enc.ciphertext.data)
        krb5_free_data_contents(context, &enc.ciphertext);
    return retval;
}