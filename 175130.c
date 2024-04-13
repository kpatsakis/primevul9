static char *get_netscape_format(const struct Cookie *co)
{
  return aprintf(
    "%s"     /* httponly preamble */
    "%s%s\t" /* domain */
    "%s\t"   /* tailmatch */
    "%s\t"   /* path */
    "%s\t"   /* secure */
    "%" CURL_FORMAT_CURL_OFF_T "\t"   /* expires */
    "%s\t"   /* name */
    "%s",    /* value */
    co->httponly?"#HttpOnly_":"",
    /* Make sure all domains are prefixed with a dot if they allow
       tailmatching. This is Mozilla-style. */
    (co->tailmatch && co->domain && co->domain[0] != '.')? ".":"",
    co->domain?co->domain:"unknown",
    co->tailmatch?"TRUE":"FALSE",
    co->path?co->path:"/",
    co->secure?"TRUE":"FALSE",
    co->expires,
    co->name,
    co->value?co->value:"");
}