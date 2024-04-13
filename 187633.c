EXPORTED char *httpdate_gen(char *buf, size_t len, time_t t)
{
    struct tm *tm = gmtime(&t);

    snprintf(buf, len, "%3s, %02d %3s %4d %02d:%02d:%02d GMT",
             wday[tm->tm_wday],
             tm->tm_mday, monthname[tm->tm_mon], tm->tm_year + 1900,
             tm->tm_hour, tm->tm_min, tm->tm_sec);

    return buf;
}