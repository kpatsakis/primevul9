ec_verify(krb5_context context, krb5_data *req_pkt, krb5_kdc_req *request,
          krb5_enc_tkt_part *enc_tkt_reply, krb5_pa_data *data,
          krb5_kdcpreauth_callbacks cb, krb5_kdcpreauth_rock rock,
          krb5_kdcpreauth_moddata moddata,
          krb5_kdcpreauth_verify_respond_fn respond, void *arg)
{
    krb5_error_code retval = 0;
    krb5_enc_data *enc = NULL;
    krb5_data scratch, plain;
    krb5_keyblock *armor_key = cb->fast_armor(context, rock);
    krb5_pa_enc_ts *ts = NULL;
    krb5_keyblock *client_keys = NULL;
    krb5_keyblock *challenge_key = NULL;
    krb5_keyblock *kdc_challenge_key;
    krb5_kdcpreauth_modreq modreq = NULL;
    int i = 0;
    char *ai = NULL, *realmstr = NULL;
    krb5_data realm = request->server->realm;

    plain.data = NULL;

    if (armor_key == NULL) {
        retval = ENOENT;
        k5_setmsg(context, ENOENT,
                  _("Encrypted Challenge used outside of FAST tunnel"));
    }
    scratch.data = (char *) data->contents;
    scratch.length = data->length;
    if (retval == 0)
        retval = decode_krb5_enc_data(&scratch, &enc);
    if (retval == 0) {
        plain.data =  malloc(enc->ciphertext.length);
        plain.length = enc->ciphertext.length;
        if (plain.data == NULL)
            retval = ENOMEM;
    }

    /* Check for a configured FAST ec auth indicator. */
    if (retval == 0)
        realmstr = k5memdup0(realm.data, realm.length, &retval);
    if (realmstr != NULL)
        retval = profile_get_string(context->profile, KRB5_CONF_REALMS,
                                    realmstr,
                                    KRB5_CONF_ENCRYPTED_CHALLENGE_INDICATOR,
                                    NULL, &ai);

    if (retval == 0)
        retval = cb->client_keys(context, rock, &client_keys);
    if (retval == 0) {
        for (i = 0; client_keys[i].enctype&& (retval == 0); i++ ) {
            retval = krb5_c_fx_cf2_simple(context,
                                          armor_key, "clientchallengearmor",
                                          &client_keys[i], "challengelongterm",
                                          &challenge_key);
            if (retval == 0)
                retval  = krb5_c_decrypt(context, challenge_key,
                                         KRB5_KEYUSAGE_ENC_CHALLENGE_CLIENT,
                                         NULL, enc, &plain);
            if (challenge_key)
                krb5_free_keyblock(context, challenge_key);
            challenge_key = NULL;
            if (retval == 0)
                break;
            /*We failed to decrypt. Try next key*/
            retval = 0;
        }
        if (client_keys[i].enctype == 0) {
            retval = KRB5KDC_ERR_PREAUTH_FAILED;
            k5_setmsg(context, retval,
                      _("Incorrect password in encrypted challenge"));
        }
    }
    if (retval == 0)
        retval = decode_krb5_pa_enc_ts(&plain, &ts);
    if (retval == 0)
        retval = krb5_check_clockskew(context, ts->patimestamp);
    if (retval == 0) {
        enc_tkt_reply->flags |= TKT_FLG_PRE_AUTH;
        /*
         * If this fails, we won't generate a reply to the client.  That may
         * cause the client to fail, but at this point the KDC has considered
         * this a success, so the return value is ignored.
         */
        if (krb5_c_fx_cf2_simple(context, armor_key, "kdcchallengearmor",
                                 &client_keys[i], "challengelongterm",
                                 &kdc_challenge_key) == 0) {
            modreq = (krb5_kdcpreauth_modreq)kdc_challenge_key;
            if (ai != NULL)
                cb->add_auth_indicator(context, rock, ai);
        }
    }
    cb->free_keys(context, rock, client_keys);
    if (plain.data)
        free(plain.data);
    if (enc)
        krb5_free_enc_data(context, enc);
    if (ts)
        krb5_free_pa_enc_ts(context, ts);
    free(realmstr);
    free(ai);

    (*respond)(arg, retval, modreq, NULL, NULL);
}