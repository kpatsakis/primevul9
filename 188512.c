void qmp_guest_set_user_password(const char *username,
                                 const char *password,
                                 bool crypted,
                                 Error **errp)
{
    NET_API_STATUS nas;
    char *rawpasswddata = NULL;
    size_t rawpasswdlen;
    wchar_t *user = NULL, *wpass = NULL;
    USER_INFO_1003 pi1003 = { 0, };
    GError *gerr = NULL;

    if (crypted) {
        error_setg(errp, QERR_UNSUPPORTED);
        return;
    }

    rawpasswddata = (char *)qbase64_decode(password, -1, &rawpasswdlen, errp);
    if (!rawpasswddata) {
        return;
    }
    rawpasswddata = g_renew(char, rawpasswddata, rawpasswdlen + 1);
    rawpasswddata[rawpasswdlen] = '\0';

    user = g_utf8_to_utf16(username, -1, NULL, NULL, &gerr);
    if (!user) {
        goto done;
    }

    wpass = g_utf8_to_utf16(rawpasswddata, -1, NULL, NULL, &gerr);
    if (!wpass) {
        goto done;
    }

    pi1003.usri1003_password = wpass;
    nas = NetUserSetInfo(NULL, user,
                         1003, (LPBYTE)&pi1003,
                         NULL);

    if (nas != NERR_Success) {
        gchar *msg = get_net_error_message(nas);
        error_setg(errp, "failed to set password: %s", msg);
        g_free(msg);
    }

done:
    if (gerr) {
        error_setg(errp, QERR_QGA_COMMAND_FAILED, gerr->message);
        g_error_free(gerr);
    }
    g_free(user);
    g_free(wpass);
    g_free(rawpasswddata);
}