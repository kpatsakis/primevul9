check_client_referral(krb5_context context,
		      krb5_kdc_rep *rep,
		      krb5_const_principal requested,
		      krb5_const_principal mapped,
		      krb5_keyblock const * key)
{
    if (krb5_principal_compare(context, requested, mapped) == FALSE &&
	!rep->enc_part.flags.enc_pa_rep)
    {
	krb5_set_error_message(context, KRB5KRB_AP_ERR_MODIFIED,
			       N_("Not same client principal returned "
				  "as requested", ""));
	return KRB5KRB_AP_ERR_MODIFIED;
    }
    return 0;
}