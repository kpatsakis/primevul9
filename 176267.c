_krb5_get_ad(krb5_context context,
	     const AuthorizationData *ad,
	     krb5_keyblock *sessionkey,
	     int type,
	     krb5_data *data)
{
    krb5_boolean found = FALSE;
    krb5_error_code ret;

    krb5_data_zero(data);

    if (ad == NULL) {
	krb5_set_error_message(context, ENOENT,
			       N_("No authorization data", ""));
	return ENOENT; /* XXX */
    }

    ret = find_type_in_ad(context, type, data, &found, TRUE, sessionkey, ad, 0);
    if (ret)
	return ret;
    if (!found) {
	krb5_set_error_message(context, ENOENT,
			       N_("Have no authorization data of type %d", ""),
			       type);
	return ENOENT; /* XXX */
    }
    return 0;
}