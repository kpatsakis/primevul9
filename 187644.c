EXPORTED int httpd_myrights(struct auth_state *authstate, const mbentry_t *mbentry)
{
    int rights = 0;

    if (mbentry && mbentry->acl) {
        rights = cyrus_acl_myrights(authstate, mbentry->acl);

        if (mbentry->mbtype == MBTYPE_CALENDAR &&
            (rights & DACL_READ) == DACL_READ) {
            rights |= DACL_READFB;
        }
    }

    return rights;
}