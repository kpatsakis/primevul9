check_s4u2self(krb5_context context,
	       krb5_kdc_configuration *config,
	       HDB *clientdb,
	       hdb_entry_ex *client,
	       krb5_const_principal server)
{
    krb5_error_code ret;

    /* if client does a s4u2self to itself, that ok */
    if (krb5_principal_compare(context, client->entry.principal, server) == TRUE)
	return 0;

    if (clientdb->hdb_check_s4u2self) {
	ret = clientdb->hdb_check_s4u2self(context, clientdb, client, server);
	if (ret == 0)
	    return 0;
    } else {
	ret = KRB5KDC_ERR_BADOPTION;
    }
    return ret;
}