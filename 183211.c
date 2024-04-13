kdcpreauth_encrypted_challenge_initvt(krb5_context context, int maj_ver,
                                      int min_ver, krb5_plugin_vtable vtable)
{
    krb5_kdcpreauth_vtable vt;

    if (maj_ver != 1)
        return KRB5_PLUGIN_VER_NOTSUPP;
    vt = (krb5_kdcpreauth_vtable)vtable;
    vt->name = "encrypted_challenge";
    vt->pa_type_list = ec_types;
    vt->edata = ec_edata;
    vt->verify = ec_verify;
    vt->return_padata = ec_return;
    return 0;
}