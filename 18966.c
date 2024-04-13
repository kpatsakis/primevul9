need_referral(krb5_context context, krb5_kdc_configuration *config,
	      const KDCOptions * const options, krb5_principal server,
	      krb5_realm **realms)
{
    const char *name;

    if(!options->canonicalize && server->name.name_type != KRB5_NT_SRV_INST)
	return FALSE;

    if (server->name.name_string.len == 1)
	name = server->name.name_string.val[0];
    else if (server->name.name_string.len == 3) {
	/*
	  This is used to give referrals for the
	  E3514235-4B06-11D1-AB04-00C04FC2DCD2/NTDSGUID/DNSDOMAIN
	  SPN form, which is used for inter-domain communication in AD
	 */
	name = server->name.name_string.val[2];
	kdc_log(context, config, 4, "Giving 3 part referral for %s", name);
	*realms = malloc(sizeof(char *)*2);
	if (*realms == NULL) {
	    krb5_set_error_message(context, ENOMEM, N_("malloc: out of memory", ""));
	    return FALSE;
	}
	(*realms)[0] = strdup(name);
	(*realms)[1] = NULL;
	return TRUE;
    } else if (server->name.name_string.len > 1)
	name = server->name.name_string.val[1];
    else
	return FALSE;

    kdc_log(context, config, 5, "Searching referral for %s", name);

    return _krb5_get_host_realm_int(context, name, FALSE, realms) == 0;
}