finish_realm(kdc_realm_t *rdp)
{
    if (rdp->realm_name)
        free(rdp->realm_name);
    if (rdp->realm_mpname)
        free(rdp->realm_mpname);
    if (rdp->realm_stash)
        free(rdp->realm_stash);
    if (rdp->realm_ports)
        free(rdp->realm_ports);
    if (rdp->realm_tcp_ports)
        free(rdp->realm_tcp_ports);
    if (rdp->realm_keytab)
        krb5_kt_close(rdp->realm_context, rdp->realm_keytab);
    if (rdp->realm_hostbased)
        free(rdp->realm_hostbased);
    if (rdp->realm_no_referral)
        free(rdp->realm_no_referral);
    if (rdp->realm_context) {
        if (rdp->realm_mprinc)
            krb5_free_principal(rdp->realm_context, rdp->realm_mprinc);
        if (rdp->realm_mkey.length && rdp->realm_mkey.contents) {
            /* XXX shouldn't memset be zap for safety? */
            memset(rdp->realm_mkey.contents, 0, rdp->realm_mkey.length);
            free(rdp->realm_mkey.contents);
        }
        krb5_db_fini(rdp->realm_context);
        if (rdp->realm_tgsprinc)
            krb5_free_principal(rdp->realm_context, rdp->realm_tgsprinc);
        krb5_free_context(rdp->realm_context);
    }
    memset(rdp, 0, sizeof(*rdp));
    free(rdp);
}