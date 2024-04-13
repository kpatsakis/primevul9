static char *make_sieve_db(const char *user)
{
    static char buf[MAX_MAILBOX_PATH+1];

    buf[0] = '.';
    buf[1] = '\0';
    strlcat(buf, user, sizeof(buf));
    strlcat(buf, ".sieve.", sizeof(buf));

    return buf;
}