static char *get_addrbook_mboxname(const char *list, const char *userid)
{
    const char *addrbook_urn_full = "urn:ietf:params:sieve:addrbook:";
    const char *addrbook_urn_abbrev = ":addrbook:";
    char *abook = NULL, *mboxname = NULL;

    /* percent-decode list URI */
    char *uri = xmlURIUnescapeString(list, strlen(list), NULL);

    if (!strncmp(uri, addrbook_urn_full, strlen(addrbook_urn_full))) {
        abook = xstrdup(uri + strlen(addrbook_urn_full));
    }
    else if (!strncmp(uri, addrbook_urn_abbrev, strlen(addrbook_urn_abbrev))) {
        abook = xstrdup(uri + strlen(addrbook_urn_abbrev));
    }

    free(uri);

    if (!abook) return NULL;

    /* MUST match default addressbook case-insensitively */
    if (!strcasecmp(abook, "Default")) {
        abook[0] = 'D';
        lcase(abook+1);
    }

    /* construct mailbox name of addressbook */
    mboxname = carddav_mboxname(userid, abook);

    /* see if addressbook mailbox exists */
    if (mboxlist_lookup(mboxname, NULL, NULL) == 0) return mboxname;
    else {
        free(mboxname);
        return NULL;
    }
}