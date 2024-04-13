deliver_init(void)
{
#ifdef EXIM_TFO_PROBE
tfo_probe();
#else
tcp_fastopen_ok = TRUE;
#endif


if (!regex_PIPELINING) regex_PIPELINING =
  regex_must_compile(US"\\n250[\\s\\-]PIPELINING(\\s|\\n|$)", FALSE, TRUE);

if (!regex_SIZE) regex_SIZE =
  regex_must_compile(US"\\n250[\\s\\-]SIZE(\\s|\\n|$)", FALSE, TRUE);

if (!regex_AUTH) regex_AUTH =
  regex_must_compile(US"\\n250[\\s\\-]AUTH\\s+([\\-\\w\\s]+)(?:\\n|$)",
    FALSE, TRUE);

#ifdef SUPPORT_TLS
if (!regex_STARTTLS) regex_STARTTLS =
  regex_must_compile(US"\\n250[\\s\\-]STARTTLS(\\s|\\n|$)", FALSE, TRUE);
#endif

if (!regex_CHUNKING) regex_CHUNKING =
  regex_must_compile(US"\\n250[\\s\\-]CHUNKING(\\s|\\n|$)", FALSE, TRUE);

#ifndef DISABLE_PRDR
if (!regex_PRDR) regex_PRDR =
  regex_must_compile(US"\\n250[\\s\\-]PRDR(\\s|\\n|$)", FALSE, TRUE);
#endif

#ifdef SUPPORT_I18N
if (!regex_UTF8) regex_UTF8 =
  regex_must_compile(US"\\n250[\\s\\-]SMTPUTF8(\\s|\\n|$)", FALSE, TRUE);
#endif

if (!regex_DSN) regex_DSN  =
  regex_must_compile(US"\\n250[\\s\\-]DSN(\\s|\\n|$)", FALSE, TRUE);

if (!regex_IGNOREQUOTA) regex_IGNOREQUOTA =
  regex_must_compile(US"\\n250[\\s\\-]IGNOREQUOTA(\\s|\\n|$)", FALSE, TRUE);
}