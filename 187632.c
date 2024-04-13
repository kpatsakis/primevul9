static int proxy_authz(const char **authzid, struct transaction_t *txn)
{
    static char authzbuf[MAX_MAILBOX_BUFFER];
    unsigned authzlen;
    int status;

    syslog(LOG_DEBUG, "proxy_auth: authzid='%s'", *authzid);

    /* Free userid & authstate previously allocated for auth'd user */
    if (httpd_userid) {
        free(httpd_userid);
        httpd_userid = NULL;
    }
    if (httpd_extrafolder) {
        free(httpd_extrafolder);
        httpd_extrafolder = NULL;
    }
    if (httpd_extradomain) {
        free(httpd_extradomain);
        httpd_extradomain = NULL;
    }
    if (httpd_authstate) {
        auth_freestate(httpd_authstate);
        httpd_authstate = NULL;
    }

    if (!(config_mupdate_server && config_getstring(IMAPOPT_PROXYSERVERS))) {
        /* Not a backend in a Murder - proxy authz is not allowed */
        syslog(LOG_NOTICE, "badlogin: %s %s %s %s",
               txn->conn->clienthost, txn->auth_chal.scheme->name, httpd_authid,
               "proxy authz attempted on non-Murder backend");
        return SASL_NOAUTHZ;
    }

    /* Canonify the authzid */
    status = mysasl_canon_user(httpd_saslconn, NULL,
                               *authzid, strlen(*authzid),
                               SASL_CU_AUTHZID, NULL,
                               authzbuf, sizeof(authzbuf), &authzlen);
    if (status) {
        syslog(LOG_NOTICE, "badlogin: %s %s %s invalid user",
               txn->conn->clienthost, txn->auth_chal.scheme->name,
               beautify_string(*authzid));
        return status;
    }

    /* See if auth'd user is allowed to proxy */
    status = mysasl_proxy_policy(httpd_saslconn, &httpd_proxyctx,
                                 authzbuf, authzlen,
                                 httpd_authid, strlen(httpd_authid),
                                 NULL, 0, NULL);

    if (status) {
        syslog(LOG_NOTICE, "badlogin: %s %s %s %s",
               txn->conn->clienthost, txn->auth_chal.scheme->name, httpd_authid,
               sasl_errdetail(httpd_saslconn));
        return status;
    }

    *authzid = authzbuf;

    return status;
}