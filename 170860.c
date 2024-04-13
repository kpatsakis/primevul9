int Curl_fnmatch(void *ptr, const char *pattern, const char *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  if(!pattern || !string) {
    return CURL_FNMATCH_FAIL;
  }
  return loop((unsigned char *)pattern, (unsigned char *)string);
}