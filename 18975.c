check_constrained_delegation(krb5_context context,
			     krb5_kdc_configuration *config,
			     HDB *clientdb,
			     hdb_entry_ex *client,
			     hdb_entry_ex *server,
			     krb5_const_principal target)
{
    const HDB_Ext_Constrained_delegation_acl *acl;
    krb5_error_code ret;
    size_t i;

    /*
     * constrained_delegation (S4U2Proxy) only works within
     * the same realm. We use the already canonicalized version
     * of the principals here, while "target" is the principal
     * provided by the client.
     */
    if(!krb5_realm_compare(context, client->entry.principal, server->entry.principal)) {
	ret = KRB5KDC_ERR_BADOPTION;
	kdc_log(context, config, 4,
	    "Bad request for constrained delegation");
	return ret;
    }

    if (clientdb->hdb_check_constrained_delegation) {
	ret = clientdb->hdb_check_constrained_delegation(context, clientdb, client, target);
	if (ret == 0)
	    return 0;
    } else {
	/* if client delegates to itself, that ok */
	if (krb5_principal_compare(context, client->entry.principal, server->entry.principal) == TRUE)
	    return 0;

	ret = hdb_entry_get_ConstrainedDelegACL(&client->entry, &acl);
	if (ret) {
	    krb5_clear_error_message(context);
	    return ret;
	}

	if (acl) {
	    for (i = 0; i < acl->len; i++) {
		if (krb5_principal_compare(context, target, &acl->val[i]) == TRUE)
		    return 0;
	    }
	}
	ret = KRB5KDC_ERR_BADOPTION;
    }
    kdc_log(context, config, 4,
	    "Bad request for constrained delegation");
    return ret;
}