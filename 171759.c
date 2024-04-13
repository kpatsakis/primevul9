find_realm_data(struct server_handle *handle, char *rname, krb5_ui_4 rsize)
{
    int i;
    kdc_realm_t **kdc_realmlist = handle->kdc_realmlist;
    int kdc_numrealms = handle->kdc_numrealms;

    for (i=0; i<kdc_numrealms; i++) {
        if ((rsize == strlen(kdc_realmlist[i]->realm_name)) &&
            !strncmp(rname, kdc_realmlist[i]->realm_name, rsize))
            return(kdc_realmlist[i]);
    }
    return((kdc_realm_t *) NULL);
}