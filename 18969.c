tgs_parse_request(astgs_request_t r,
		  const PA_DATA *tgs_req,
		  hdb_entry_ex **krbtgt,
		  krb5_enctype *krbtgt_etype,
		  krb5_ticket **ticket,
		  const char **e_text,
		  const char *from,
		  const struct sockaddr *from_addr,
		  time_t **csec,
		  int **cusec,
		  AuthorizationData **auth_data,
		  krb5_keyblock **replykey,
		  int *rk_is_subkey)
{
    krb5_context context = r->context;
    krb5_kdc_configuration *config = r->config;
    KDC_REQ_BODY *b = &r->req.req_body;
    static char failed[] = "<unparse_name failed>";
    krb5_ap_req ap_req;
    krb5_error_code ret;
    krb5_principal princ;
    krb5_auth_context ac = NULL;
    krb5_flags ap_req_options;
    krb5_flags verify_ap_req_flags = 0;
    krb5_crypto crypto;
    krb5uint32 krbtgt_kvno;     /* kvno used for the PA-TGS-REQ AP-REQ Ticket */
    krb5uint32 krbtgt_kvno_try;
    int kvno_search_tries = 4;  /* number of kvnos to try when tkt_vno == 0 */
    const Keys *krbtgt_keys;/* keyset for TGT tkt_vno */
    Key *tkey;
    krb5_keyblock *subkey = NULL;
    unsigned usage;

    *auth_data = NULL;
    *csec  = NULL;
    *cusec = NULL;
    *replykey = NULL;

    memset(&ap_req, 0, sizeof(ap_req));
    ret = krb5_decode_ap_req(context, &tgs_req->padata_value, &ap_req);
    if(ret){
	const char *msg = krb5_get_error_message(context, ret);
	kdc_log(context, config, 4, "Failed to decode AP-REQ: %s", msg);
	krb5_free_error_message(context, msg);
	goto out;
    }

    if(!get_krbtgt_realm(&ap_req.ticket.sname)){
	/* XXX check for ticket.sname == req.sname */
	kdc_log(context, config, 4, "PA-DATA is not a ticket-granting ticket");
	ret = KRB5KDC_ERR_POLICY; /* ? */
	goto out;
    }

    _krb5_principalname2krb5_principal(context,
				       &princ,
				       ap_req.ticket.sname,
				       ap_req.ticket.realm);

    krbtgt_kvno = ap_req.ticket.enc_part.kvno ? *ap_req.ticket.enc_part.kvno : 0;
    ret = _kdc_db_fetch(context, config, princ, HDB_F_GET_KRBTGT,
			&krbtgt_kvno, NULL, krbtgt);

    if (ret == HDB_ERR_NOT_FOUND_HERE) {
	/* XXX Factor out this unparsing of the same princ all over */
	char *p;
	ret = krb5_unparse_name(context, princ, &p);
	if (ret != 0)
	    p = failed;
	krb5_free_principal(context, princ);
	kdc_log(context, config, 5,
		"Ticket-granting ticket account %s does not have secrets at "
		"this KDC, need to proxy", p);
	if (ret == 0)
	    free(p);
	ret = HDB_ERR_NOT_FOUND_HERE;
	goto out;
    } else if (ret == HDB_ERR_KVNO_NOT_FOUND) {
	char *p;
	ret = krb5_unparse_name(context, princ, &p);
	if (ret != 0)
	    p = failed;
	krb5_free_principal(context, princ);
	kdc_log(context, config, 5,
		"Ticket-granting ticket account %s does not have keys for "
		"kvno %d at this KDC", p, krbtgt_kvno);
	if (ret == 0)
	    free(p);
	ret = HDB_ERR_KVNO_NOT_FOUND;
	goto out;
    } else if (ret == HDB_ERR_NO_MKEY) {
	char *p;
	ret = krb5_unparse_name(context, princ, &p);
	if (ret != 0)
	    p = failed;
	krb5_free_principal(context, princ);
	kdc_log(context, config, 5,
		"Missing master key for decrypting keys for ticket-granting "
		"ticket account %s with kvno %d at this KDC", p, krbtgt_kvno);
	if (ret == 0)
	    free(p);
	ret = HDB_ERR_KVNO_NOT_FOUND;
	goto out;
    } else if (ret) {
	const char *msg = krb5_get_error_message(context, ret);
	char *p;
	ret = krb5_unparse_name(context, princ, &p);
	if (ret != 0)
	    p = failed;
	kdc_log(context, config, 4,
		"Ticket-granting ticket %s not found in database: %s", p, msg);
	krb5_free_principal(context, princ);
	krb5_free_error_message(context, msg);
	if (ret == 0)
	    free(p);
	ret = KRB5KRB_AP_ERR_NOT_US;
	goto out;
    }

    krbtgt_kvno_try = krbtgt_kvno ? krbtgt_kvno : (*krbtgt)->entry.kvno;
    *krbtgt_etype = ap_req.ticket.enc_part.etype;

next_kvno:
    krbtgt_keys = hdb_kvno2keys(context, &(*krbtgt)->entry, krbtgt_kvno_try);
    ret = hdb_enctype2key(context, &(*krbtgt)->entry, krbtgt_keys,
			  ap_req.ticket.enc_part.etype, &tkey);
    if (ret && krbtgt_kvno == 0 && kvno_search_tries > 0) {
	kvno_search_tries--;
	krbtgt_kvno_try--;
	goto next_kvno;
    } else if (ret) {
	char *str = NULL, *p = NULL;

	krb5_enctype_to_string(context, ap_req.ticket.enc_part.etype, &str);
	krb5_unparse_name(context, princ, &p);
	kdc_log(context, config, 4,
		"No server key with enctype %s found for %s",
		str ? str : "<unknown enctype>",
		p ? p : "<unparse_name failed>");
	free(str);
	free(p);
	ret = KRB5KRB_AP_ERR_BADKEYVER;
	goto out;
    }

    if (b->kdc_options.validate)
	verify_ap_req_flags |= KRB5_VERIFY_AP_REQ_IGNORE_INVALID;

    if (r->config->warn_ticket_addresses)
        verify_ap_req_flags |= KRB5_VERIFY_AP_REQ_IGNORE_ADDRS;

    ret = krb5_verify_ap_req2(context,
			      &ac,
			      &ap_req,
			      princ,
			      &tkey->key,
			      verify_ap_req_flags,
			      &ap_req_options,
			      ticket,
			      KRB5_KU_TGS_REQ_AUTH);
    if (ticket && *ticket && (*ticket)->ticket.caddr)
        _kdc_audit_addaddrs((kdc_request_t)r, (*ticket)->ticket.caddr, "tixaddrs");
    if (r->config->warn_ticket_addresses && ret == KRB5KRB_AP_ERR_BADADDR &&
        *ticket != NULL) {
        _kdc_audit_addkv((kdc_request_t)r, 0, "wrongaddr", "yes");
        ret = 0;
    }
    if (ret == KRB5KRB_AP_ERR_BAD_INTEGRITY && kvno_search_tries > 0) {
	kvno_search_tries--;
	krbtgt_kvno_try--;
	goto next_kvno;
    }

    krb5_free_principal(context, princ);
    if(ret) {
	const char *msg = krb5_get_error_message(context, ret);
	kdc_log(context, config, 4, "Failed to verify AP-REQ: %s", msg);
	krb5_free_error_message(context, msg);
	goto out;
    }

    {
	krb5_authenticator auth;

	ret = krb5_auth_con_getauthenticator(context, ac, &auth);
	if (ret == 0) {
	    *csec   = malloc(sizeof(**csec));
	    if (*csec == NULL) {
		krb5_free_authenticator(context, &auth);
		kdc_log(context, config, 4, "malloc failed");
		goto out;
	    }
	    **csec  = auth->ctime;
	    *cusec  = malloc(sizeof(**cusec));
	    if (*cusec == NULL) {
		krb5_free_authenticator(context, &auth);
		kdc_log(context, config, 4, "malloc failed");
		goto out;
	    }
	    **cusec  = auth->cusec;
	    krb5_free_authenticator(context, &auth);
	}
    }

    ret = tgs_check_authenticator(context, config,
				  ac, b, e_text, &(*ticket)->ticket.key);
    if (ret) {
	krb5_auth_con_free(context, ac);
	goto out;
    }

    usage = KRB5_KU_TGS_REQ_AUTH_DAT_SUBKEY;
    *rk_is_subkey = 1;

    ret = krb5_auth_con_getremotesubkey(context, ac, &subkey);
    if(ret){
	const char *msg = krb5_get_error_message(context, ret);
	krb5_auth_con_free(context, ac);
	kdc_log(context, config, 4, "Failed to get remote subkey: %s", msg);
	krb5_free_error_message(context, msg);
	goto out;
    }
    if(subkey == NULL){
	usage = KRB5_KU_TGS_REQ_AUTH_DAT_SESSION;
	*rk_is_subkey = 0;

	ret = krb5_auth_con_getkey(context, ac, &subkey);
	if(ret) {
	    const char *msg = krb5_get_error_message(context, ret);
	    krb5_auth_con_free(context, ac);
	    kdc_log(context, config, 4, "Failed to get session key: %s", msg);
	    krb5_free_error_message(context, msg);
	    goto out;
	}
    }
    if(subkey == NULL){
	krb5_auth_con_free(context, ac);
	kdc_log(context, config, 4,
		"Failed to get key for enc-authorization-data");
	ret = KRB5KRB_AP_ERR_BAD_INTEGRITY; /* ? */
	goto out;
    }

    *replykey = subkey;

    if (b->enc_authorization_data) {
	krb5_data ad;

	ret = krb5_crypto_init(context, subkey, 0, &crypto);
	if (ret) {
	    const char *msg = krb5_get_error_message(context, ret);
	    krb5_auth_con_free(context, ac);
	    kdc_log(context, config, 4, "krb5_crypto_init failed: %s", msg);
	    krb5_free_error_message(context, msg);
	    goto out;
	}
	ret = krb5_decrypt_EncryptedData (context,
					  crypto,
					  usage,
					  b->enc_authorization_data,
					  &ad);
	krb5_crypto_destroy(context, crypto);
	if(ret){
	    krb5_auth_con_free(context, ac);
	    kdc_log(context, config, 4,
		    "Failed to decrypt enc-authorization-data");
	    ret = KRB5KRB_AP_ERR_BAD_INTEGRITY; /* ? */
	    goto out;
	}
	ALLOC(*auth_data);
	if (*auth_data == NULL) {
	    krb5_auth_con_free(context, ac);
	    ret = KRB5KRB_AP_ERR_BAD_INTEGRITY; /* ? */
	    goto out;
	}
	ret = decode_AuthorizationData(ad.data, ad.length, *auth_data, NULL);
	if(ret){
	    krb5_auth_con_free(context, ac);
	    free(*auth_data);
	    *auth_data = NULL;
	    kdc_log(context, config, 4, "Failed to decode authorization data");
	    ret = KRB5KRB_AP_ERR_BAD_INTEGRITY; /* ? */
	    goto out;
	}
    }

    krb5_auth_con_free(context, ac);

out:
    free_AP_REQ(&ap_req);

    return ret;
}