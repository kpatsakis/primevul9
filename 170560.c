static int getenvironment(void *sc __attribute__((unused)),
                          const char *keyname, char **res)
{
    *res = NULL;

    switch (*keyname) {
    case 'd':
        if (!strcmp(keyname, "domain")) {
            const char *domain = strchr(config_servername, '.');

            if (domain) domain++;
            else domain = "";

            *res = xstrdup(domain);
        }
        break;

    case 'h':
        if (!strcmp(keyname, "host")) *res = xstrdup(config_servername);
        break;

    case 'l':
        if (!strcmp(keyname, "location")) *res = xstrdup("MDA");
        break;

    case 'n':
        if (!strcmp(keyname, "name")) *res = xstrdup("Cyrus LMTP");
        break;

    case 'p':
        if (!strcmp(keyname, "phase")) *res = xstrdup("during");
        break;

    case 'r':
        if (!strncmp(keyname, "remote-", 7)) {
            const char *localip, *remoteip,
                *remotehost = get_clienthost(0, &localip, &remoteip);

            if (!strcmp(keyname+7, "host"))
                *res = xstrndup(remotehost, strcspn(remotehost, " ["));
            else if (remoteip && !strcmp(keyname+7, "ip"))
                *res = xstrndup(remoteip, strcspn(remoteip, ";"));
        }
        break;

    case 'v':
        if (!strcmp(keyname, "version")) *res = xstrdup(CYRUS_VERSION);
        break;
    }

    return (*res ? SIEVE_OK : SIEVE_FAIL);
}