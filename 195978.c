int parse_host_port(struct sockaddr_in *saddr, const char *str,
                    Error **errp)
{
    gchar **substrings;
    struct hostent *he;
    const char *addr, *p, *r;
    int port, ret = 0;

    substrings = g_strsplit(str, ":", 2);
    if (!substrings || !substrings[0] || !substrings[1]) {
        error_setg(errp, "host address '%s' doesn't contain ':' "
                   "separating host from port", str);
        ret = -1;
        goto out;
    }

    addr = substrings[0];
    p = substrings[1];

    saddr->sin_family = AF_INET;
    if (addr[0] == '\0') {
        saddr->sin_addr.s_addr = 0;
    } else {
        if (qemu_isdigit(addr[0])) {
            if (!inet_aton(addr, &saddr->sin_addr)) {
                error_setg(errp, "host address '%s' is not a valid "
                           "IPv4 address", addr);
                ret = -1;
                goto out;
            }
        } else {
            he = gethostbyname(addr);
            if (he == NULL) {
                error_setg(errp, "can't resolve host address '%s'", addr);
                ret = -1;
                goto out;
            }
            saddr->sin_addr = *(struct in_addr *)he->h_addr;
        }
    }
    port = strtol(p, (char **)&r, 0);
    if (r == p) {
        error_setg(errp, "port number '%s' is invalid", p);
        ret = -1;
        goto out;
    }
    saddr->sin_port = htons(port);

out:
    g_strfreev(substrings);
    return ret;
}