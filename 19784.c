CURLcode Curl_init_userdefined(struct UserDefined *set)
{
  CURLcode result = CURLE_OK;

  set->out = stdout; /* default output to stdout */
  set->in  = stdin;  /* default input from stdin */
  set->err  = stderr;  /* default stderr to stderr */

  /* use fwrite as default function to store output */
  set->fwrite_func = (curl_write_callback)fwrite;

  /* use fread as default function to read input */
  set->fread_func = (curl_read_callback)fread;
  set->is_fread_set = 0;
  set->is_fwrite_set = 0;

  set->seek_func = ZERO_NULL;
  set->seek_client = ZERO_NULL;

  /* conversion callbacks for non-ASCII hosts */
  set->convfromnetwork = ZERO_NULL;
  set->convtonetwork   = ZERO_NULL;
  set->convfromutf8    = ZERO_NULL;

  set->filesize = -1;        /* we don't know the size */
  set->postfieldsize = -1;   /* unknown size */
  set->maxredirs = -1;       /* allow any amount by default */

  set->httpreq = HTTPREQ_GET; /* Default HTTP request */
  set->rtspreq = RTSPREQ_OPTIONS; /* Default RTSP request */
  set->ftp_use_epsv = TRUE;   /* FTP defaults to EPSV operations */
  set->ftp_use_eprt = TRUE;   /* FTP defaults to EPRT operations */
  set->ftp_use_pret = FALSE;  /* mainly useful for drftpd servers */
  set->ftp_filemethod = FTPFILE_MULTICWD;

  set->dns_cache_timeout = 60; /* Timeout every 60 seconds by default */

  /* Set the default size of the SSL session ID cache */
  set->ssl.max_ssl_sessions = 5;

  set->proxyport = CURL_DEFAULT_PROXY_PORT; /* from url.h */
  set->proxytype = CURLPROXY_HTTP; /* defaults to HTTP proxy */
  set->httpauth = CURLAUTH_BASIC;  /* defaults to basic */
  set->proxyauth = CURLAUTH_BASIC; /* defaults to basic */

  /* make libcurl quiet by default: */
  set->hide_progress = TRUE;  /* CURLOPT_NOPROGRESS changes these */

  /*
   * libcurl 7.10 introduced SSL verification *by default*! This needs to be
   * switched off unless wanted.
   */
  set->ssl.verifypeer = TRUE;
  set->ssl.verifyhost = TRUE;
#ifdef USE_TLS_SRP
  set->ssl.authtype = CURL_TLSAUTH_NONE;
#endif
  set->ssh_auth_types = CURLSSH_AUTH_DEFAULT; /* defaults to any auth
                                                      type */
  set->ssl.sessionid = TRUE; /* session ID caching enabled by default */

  set->new_file_perms = 0644;    /* Default permissions */
  set->new_directory_perms = 0755; /* Default permissions */

  /* for the *protocols fields we don't use the CURLPROTO_ALL convenience
     define since we internally only use the lower 16 bits for the passed
     in bitmask to not conflict with the private bits */
  set->allowed_protocols = CURLPROTO_ALL;
  set->redir_protocols = CURLPROTO_ALL &  /* All except FILE, SCP and SMB */
                          ~(CURLPROTO_FILE | CURLPROTO_SCP | CURLPROTO_SMB |
                            CURLPROTO_SMBS);

#if defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /*
   * disallow unprotected protection negotiation NEC reference implementation
   * seem not to follow rfc1961 section 4.3/4.4
   */
  set->socks5_gssapi_nec = FALSE;
  /* set default GSS-API service name */
  result = setstropt(&set->str[STRING_SOCKS5_GSSAPI_SERVICE],
                     (char *) CURL_DEFAULT_SOCKS5_GSSAPI_SERVICE);
  if(result)
    return result;
#endif

  /* This is our preferred CA cert bundle/path since install time */
#if defined(CURL_CA_BUNDLE)
  result = setstropt(&set->str[STRING_SSL_CAFILE], (char *) CURL_CA_BUNDLE);
#elif defined(CURL_CA_PATH)
  result = setstropt(&set->str[STRING_SSL_CAPATH], (char *) CURL_CA_PATH);
#endif

  set->wildcardmatch  = FALSE;
  set->chunk_bgn      = ZERO_NULL;
  set->chunk_end      = ZERO_NULL;

  /* tcp keepalives are disabled by default, but provide reasonable values for
   * the interval and idle times.
   */
  set->tcp_keepalive = FALSE;
  set->tcp_keepintvl = 60;
  set->tcp_keepidle = 60;

  set->ssl_enable_npn = TRUE;
  set->ssl_enable_alpn = TRUE;

  set->expect_100_timeout = 1000L; /* Wait for a second by default. */
  return result;
}