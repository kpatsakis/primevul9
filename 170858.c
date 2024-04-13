static int loop(const unsigned char *pattern, const unsigned char *string)
{
  loop_state state = CURLFNM_LOOP_DEFAULT;
  unsigned char *p = (unsigned char *)pattern;
  unsigned char *s = (unsigned char *)string;
  unsigned char charset[CURLFNM_CHSET_SIZE] = { 0 };
  int rc = 0;

  for(;;) {
    switch(state) {
    case CURLFNM_LOOP_DEFAULT:
      if(*p == '*') {
        while(*(p + 1) == '*') /* eliminate multiple stars */
          p++;
        if(*s == '\0' && *(p + 1) == '\0')
          return CURL_FNMATCH_MATCH;
        rc = loop(p + 1, s); /* *.txt matches .txt <=> .txt matches .txt */
        if(rc == CURL_FNMATCH_MATCH)
          return CURL_FNMATCH_MATCH;
        if(*s) /* let the star eat up one character */
          s++;
        else
          return CURL_FNMATCH_NOMATCH;
      }
      else if(*p == '?') {
        if(ISPRINT(*s)) {
          s++;
          p++;
        }
        else if(*s == '\0')
          return CURL_FNMATCH_NOMATCH;
        else
          return CURL_FNMATCH_FAIL; /* cannot deal with other character */
      }
      else if(*p == '\0') {
        if(*s == '\0')
          return CURL_FNMATCH_MATCH;
        return CURL_FNMATCH_NOMATCH;
      }
      else if(*p == '\\') {
        state = CURLFNM_LOOP_BACKSLASH;
        p++;
      }
      else if(*p == '[') {
        unsigned char *pp = p + 1; /* cannot handle with pointer to register */
        if(setcharset(&pp, charset)) {
          int found = FALSE;
          if(charset[(unsigned int)*s])
            found = TRUE;
          else if(charset[CURLFNM_ALNUM])
            found = ISALNUM(*s);
          else if(charset[CURLFNM_ALPHA])
            found = ISALPHA(*s);
          else if(charset[CURLFNM_DIGIT])
            found = ISDIGIT(*s);
          else if(charset[CURLFNM_XDIGIT])
            found = ISXDIGIT(*s);
          else if(charset[CURLFNM_PRINT])
            found = ISPRINT(*s);
          else if(charset[CURLFNM_SPACE])
            found = ISSPACE(*s);
          else if(charset[CURLFNM_UPPER])
            found = ISUPPER(*s);
          else if(charset[CURLFNM_LOWER])
            found = ISLOWER(*s);
          else if(charset[CURLFNM_BLANK])
            found = ISBLANK(*s);
          else if(charset[CURLFNM_GRAPH])
            found = ISGRAPH(*s);

          if(charset[CURLFNM_NEGATE])
            found = !found;

          if(found) {
            p = pp + 1;
            s++;
            memset(charset, 0, CURLFNM_CHSET_SIZE);
          }
          else
            return CURL_FNMATCH_NOMATCH;
        }
        else
          return CURL_FNMATCH_FAIL;
      }
      else {
        if(*p++ != *s++)
          return CURL_FNMATCH_NOMATCH;
      }
      break;
    case CURLFNM_LOOP_BACKSLASH:
      if(ISPRINT(*p)) {
        if(*p++ == *s++)
          state = CURLFNM_LOOP_DEFAULT;
        else
          return CURL_FNMATCH_NOMATCH;
      }
      else
        return CURL_FNMATCH_FAIL;
      break;
    }
  }
}