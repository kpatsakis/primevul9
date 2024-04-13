static CURLcode fix_hostname(struct connectdata *conn, struct hostname *host)
{
  size_t len;
  struct Curl_easy *data = conn->data;

#ifndef USE_LIBIDN2
  (void)data;
  (void)conn;
#elif defined(CURL_DISABLE_VERBOSE_STRINGS)
  (void)conn;
#endif

  /* set the name we use to display the host name */
  host->dispname = host->name;

  len = strlen(host->name);
  if(len && (host->name[len-1] == '.'))
    /* strip off a single trailing dot if present, primarily for SNI but
       there's no use for it */
    host->name[len-1] = 0;

  /* Check name for non-ASCII and convert hostname to ACE form if we can */
  if(!is_ASCII_name(host->name)) {
#ifdef USE_LIBIDN2
    if(idn2_check_version(IDN2_VERSION)) {
      char *ace_hostname = NULL;
#if IDN2_VERSION_NUMBER >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      int flags = IDN2_NFC_INPUT | IDN2_NONTRANSITIONAL;
#else
      int flags = IDN2_NFC_INPUT;
#endif
      int rc = idn2_lookup_ul((const char *)host->name, &ace_hostname, flags);
      if(rc == IDN2_OK) {
        host->encalloc = (char *)ace_hostname;
        /* change the name pointer to point to the encoded hostname */
        host->name = host->encalloc;
      }
      else {
        failf(data, "Failed to convert %s to ACE; %s\n", host->name,
              idn2_strerror(rc));
        return CURLE_URL_MALFORMAT;
      }
    }
#elif defined(USE_WIN32_IDN)
    char *ace_hostname = NULL;

    if(curl_win32_idn_to_ascii(host->name, &ace_hostname)) {
      host->encalloc = ace_hostname;
      /* change the name pointer to point to the encoded hostname */
      host->name = host->encalloc;
    }
    else {
      failf(data, "Failed to convert %s to ACE;\n", host->name);
      return CURLE_URL_MALFORMAT;
    }
#else
    infof(data, "IDN support not present, can't parse Unicode domains\n");
#endif
  }
  {
    char *hostp;
    for(hostp = host->name; *hostp; hostp++) {
      if(*hostp <= 32) {
        failf(data, "Host name '%s' contains bad letter", host->name);
        return CURLE_URL_MALFORMAT;
      }
    }
  }
  return CURLE_OK;
}