static int http_auth(const char *creds, struct transaction_t *txn)
{
    struct auth_challenge_t *chal = &txn->auth_chal;
    static int status = SASL_OK;
    int slen, r;
    const char *clientin = NULL, *realm = NULL, *user, **authzid;
    unsigned int clientinlen = 0;
    struct auth_scheme_t *scheme;
    static char base64[BASE64_BUF_SIZE+1];
    const void *canon_user = NULL;

    /* Split credentials into auth scheme and response */
    slen = strcspn(creds, " ");
    if ((clientin = strchr(creds + slen, ' '))) {
        while (strchr(" ", *++clientin));  /* Trim leading 1*SP */
        clientinlen = strlen(clientin);
    }

    syslog(LOG_DEBUG,
           "http_auth: status=%d   scheme='%s'   creds='%.*s%s'",
           status, chal->scheme ? chal->scheme->name : "",
           slen, creds, clientin ? " <response>" : "");

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
    chal->param = NULL;

    if (chal->scheme) {
        /* Use current scheme, if possible */
        scheme = chal->scheme;

        if (strncasecmp(scheme->name, creds, slen)) {
            /* Changing auth scheme -> reset state */
            syslog(LOG_DEBUG, "http_auth: changing scheme");
            reset_saslconn(&httpd_saslconn);
            chal->scheme = NULL;
            status = SASL_OK;
        }
    }

    if (!chal->scheme) {
        /* Find the client-specified auth scheme */
        syslog(LOG_DEBUG, "http_auth: find client scheme");
        for (scheme = auth_schemes; scheme->name; scheme++) {
            if (slen && !strncasecmp(scheme->name, creds, slen)) {
                /* Found a supported scheme, see if its available */
                if (!(avail_auth_schemes & scheme->id)) scheme = NULL;
                break;
            }
        }
        if (!scheme || !scheme->name) {
            /* Didn't find a matching scheme that is available */
            syslog(LOG_DEBUG, "Unknown auth scheme '%.*s'", slen, creds);
            return SASL_NOMECH;
        }
        /* We found it! */
        syslog(LOG_DEBUG, "http_auth: found matching scheme: %s", scheme->name);
        chal->scheme = scheme;
        status = SASL_OK;

        if (!clientin && (scheme->flags & AUTH_REALM_PARAM)) {
            /* Get realm - based on namespace of URL */
            switch (txn->req_tgt.namespace->id) {
            case URL_NS_DEFAULT:
            case URL_NS_PRINCIPAL:
                realm = config_getstring(IMAPOPT_DAV_REALM);
                break;

            case URL_NS_CALENDAR:
                realm = config_getstring(IMAPOPT_CALDAV_REALM);
                break;

            case URL_NS_ADDRESSBOOK:
                realm = config_getstring(IMAPOPT_CARDDAV_REALM);
                break;

            case URL_NS_RSS:
                realm = config_getstring(IMAPOPT_RSS_REALM);
                break;
            }
            if (!realm) realm = config_servername;

            /* Create initial challenge (base64 buffer is static) */
            snprintf(base64, BASE64_BUF_SIZE, "realm=\"%s\"", realm);
            chal->param = base64;
            chal->scheme = NULL;  /* make sure we don't reset the SASL ctx */
            return status;
        }
    }

    /* Parse any auth parameters, if necessary */
    if (clientin && (scheme->flags & AUTH_DATA_PARAM)) {
        const char *sid = NULL;
        unsigned int sid_len;

        r = http_parse_auth_params(clientin, NULL /* realm */, NULL,
                                   &sid, &sid_len, &clientin, &clientinlen);
        if (r != SASL_OK) return r;

        if (sid) {
            const char *mysid = session_id();

            if (sid_len != strlen(mysid) ||
                strncmp(mysid, sid, sid_len)) {
                syslog(LOG_ERR, "%s: Incorrect 'sid' parameter in credentials",
                       scheme->name);
                return SASL_BADAUTH;
            }
        }
    }

    /* Base64 decode any client response, if necessary */
    if (clientin && (scheme->flags & AUTH_BASE64)) {
        r = sasl_decode64(clientin, clientinlen,
                          base64, BASE64_BUF_SIZE, &clientinlen);
        if (r != SASL_OK) {
            syslog(LOG_ERR, "Base64 decode failed: %s",
                   sasl_errstring(r, NULL, NULL));
            return r;
        }
        clientin = base64;
    }

    if (scheme->id == AUTH_BASIC) {
        /* Basic (plaintext) authentication */
        char *pass;
        char *extra;
        char *plus;
        char *domain;

        /* Split credentials into <user> ':' <pass>.
         * We are working with base64 buffer, so we can modify it.
         */
        user = base64;
        pass = strchr(base64, ':');
        if (!pass) {
            syslog(LOG_ERR, "Basic auth: Missing password");
            return SASL_BADPARAM;
        }
        *pass++ = '\0';
        domain = strchr(user, '@');
        if (domain) *domain++ = '\0';
        extra = strchr(user, '%');
        if (extra) *extra++ = '\0';
        plus = strchr(user, '+');
        if (plus) *plus++ = '\0';

        /* Verify the password */
        char *realuser =
            domain ? strconcat(user, "@", domain, (char *) NULL) : xstrdup(user);

        status = sasl_checkpass(httpd_saslconn, realuser, strlen(realuser),
                                pass, strlen(pass));
        memset(pass, 0, strlen(pass));          /* erase plaintext password */

        if (status) {
            syslog(LOG_NOTICE, "badlogin: %s Basic %s %s",
                   txn->conn->clienthost, realuser,
                   sasl_errdetail(httpd_saslconn));
            free(realuser);

            /* Don't allow user probing */
            if (status == SASL_NOUSER) status = SASL_BADAUTH;
            return status;
        }
        free(realuser);

        /* Successful authentication - fall through */
        httpd_extrafolder = xstrdupnull(plus);
        httpd_extradomain = xstrdupnull(extra);
    }
    else if (scheme->id == AUTH_BEARER) {
        /* Bearer authentication */
        assert(txn->req_tgt.namespace->bearer);

        /* Call namespace bearer authentication.
         * We are working with base64 buffer, so the namespace can
         * write the canonicalized userid into the buffer */
        base64[0] = 0;
        status = txn->req_tgt.namespace->bearer(clientin,
                                                base64, BASE64_BUF_SIZE);
        if (status) return status;
        canon_user = user = base64;

        /* Successful authentication - fall through */
        httpd_extrafolder = NULL;
        httpd_extradomain = NULL;
        httpd_authstate = auth_newstate(user);
    }
    else {
        /* SASL-based authentication (SCRAM_*, Digest, Negotiate, NTLM) */
        const char *serverout = NULL;
        unsigned int serveroutlen = 0;
        unsigned int auth_params_len = 0;

#ifdef SASL_HTTP_REQUEST
        /* Setup SASL HTTP request, if necessary */
        sasl_http_request_t sasl_http_req;

        if (scheme->flags & AUTH_NEED_REQUEST) {
            sasl_http_req.method = txn->req_line.meth;
            sasl_http_req.uri = txn->req_line.uri;
            sasl_http_req.entity = NULL;
            sasl_http_req.elen = 0;
            sasl_http_req.non_persist = txn->flags.conn & CONN_CLOSE;
            sasl_setprop(httpd_saslconn, SASL_HTTP_REQUEST, &sasl_http_req);
        }
#endif /* SASL_HTTP_REQUEST */

        if (status == SASL_CONTINUE) {
            /* Continue current authentication exchange */
            syslog(LOG_DEBUG, "http_auth: continue %s", scheme->saslmech);
            status = sasl_server_step(httpd_saslconn, clientin, clientinlen,
                                      &serverout, &serveroutlen);
        }
        else {
            /* Start new authentication exchange */
            syslog(LOG_DEBUG, "http_auth: start %s", scheme->saslmech);
            status = sasl_server_start(httpd_saslconn, scheme->saslmech,
                                       clientin, clientinlen,
                                       &serverout, &serveroutlen);
        }

        /* Failure - probably bad client response */
        if ((status != SASL_OK) && (status != SASL_CONTINUE)) {
            syslog(LOG_ERR, "SASL failed: %s",
                   sasl_errstring(status, NULL, NULL));
            return status;
        }

        /* Prepend any auth parameters, if necessary */
        if (scheme->flags & AUTH_DATA_PARAM) {
            auth_params_len = snprintf(base64,
                                       MAX_AUTHPARAM_SIZE + MAX_SESSIONID_SIZE,
                                       "sid=%s%s", session_id(),
                                       serverout ? ",data=" : "");
        }

        /* Base64 encode any server challenge, if necessary */
        if (serverout && (scheme->flags & AUTH_BASE64)) {
            r = sasl_encode64(serverout, serveroutlen,
                              base64 + auth_params_len, MAX_BASE64_SIZE, NULL);
            if (r != SASL_OK) {
                syslog(LOG_ERR, "Base64 encode failed: %s",
                       sasl_errstring(r, NULL, NULL));
                return r;
            }
            serverout = base64;
        }

        chal->param = serverout;

        if (status == SASL_CONTINUE) {
            /* Need another step to complete authentication */
            return status;
        }

        /* Successful authentication
         *
         * HTTP doesn't support security layers,
         * so don't attach SASL context to prot layer.
         */
    }

    if (!canon_user) {
        /* Get the userid from SASL - already canonicalized */
        status = sasl_getprop(httpd_saslconn, SASL_USERNAME, &canon_user);
        if (status != SASL_OK) {
            syslog(LOG_ERR, "weird SASL error %d getting SASL_USERNAME", status);
            return status;
        }
        user = (const char *) canon_user;
    }

    if (httpd_authid) free(httpd_authid);
    httpd_authid = xstrdup(user);

    authzid = spool_getheader(txn->req_hdrs, "Authorize-As");
    if (authzid && *authzid[0]) {
        /* Trying to proxy as another user */
        user = authzid[0];

        status = proxy_authz(&user, txn);
        if (status) return status;
    }

    /* Post-process the successful authentication. */
    r = auth_success(txn, user);
    if (r == HTTP_UNAVAILABLE) {
        status = SASL_UNAVAIL;
    }
    else if (r) {
        /* Any error here comes after the user already logged in,
         * so avoid to return SASL_BADAUTH. It would trigger the
         * HTTP handler to send UNAUTHORIZED, and might confuse
         * users that provided their correct credentials. */
        syslog(LOG_ERR, "auth_success returned error: %s", error_message(r));
        status = SASL_FAIL;
    }

    return status;
}