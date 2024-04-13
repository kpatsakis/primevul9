int parse_host_port(struct sockaddr_in *saddr, const char *str,
                    Error **errp)
{
    char buf[512];
    struct hostent *he;
    const char *p, *r;
    int port;

    p = str;
    if (get_str_sep(buf, sizeof(buf), &p, ':') < 0) {
        error_setg(errp, "host address '%s' doesn't contain ':' "
                   "separating host from port", str);
        return -1;
    }
    saddr->sin_family = AF_INET;
    if (buf[0] == '\0') {
        saddr->sin_addr.s_addr = 0;
    } else {
        if (qemu_isdigit(buf[0])) {
            if (!inet_aton(buf, &saddr->sin_addr)) {
                error_setg(errp, "host address '%s' is not a valid "
                           "IPv4 address", buf);
                return -1;
            }
        } else {
            he = gethostbyname(buf);
            if (he == NULL) {
                error_setg(errp, "can't resolve host address '%s'", buf);
                return - 1;
            }
            saddr->sin_addr = *(struct in_addr *)he->h_addr;
        }
    }
    port = strtol(p, (char **)&r, 0);
    if (r == p) {
        error_setg(errp, "port number '%s' is invalid", p);
        return -1;
    }
    saddr->sin_port = htons(port);
    return 0;
}