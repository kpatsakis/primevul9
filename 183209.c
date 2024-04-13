ec_edata(krb5_context context, krb5_kdc_req *request,
         krb5_kdcpreauth_callbacks cb, krb5_kdcpreauth_rock rock,
         krb5_kdcpreauth_moddata moddata, krb5_preauthtype pa_type,
         krb5_kdcpreauth_edata_respond_fn respond, void *arg)
{
    krb5_keyblock *armor_key = cb->fast_armor(context, rock);

    /* Encrypted challenge only works with FAST, and requires a client key. */
    if (armor_key == NULL || !cb->have_client_keys(context, rock))
        (*respond)(arg, ENOENT, NULL);
    else
        (*respond)(arg, 0, NULL);
}