Curl_cookie_add(struct SessionHandle *data,
                /* The 'data' pointer here may be NULL at times, and thus
                   must only be used very carefully for things that can deal
                   with data being NULL. Such as infof() and similar */

                struct CookieInfo *c,
                bool httpheader, /* TRUE if HTTP header-style line */
                char *lineptr,   /* first character of the line */
                const char *domain, /* default domain */
                const char *path)   /* full path used when this cookie is set,
                                       used to get default path for the cookie
                                       unless set */
{
  struct Cookie *clist;
  char name[MAX_NAME];
  struct Cookie *co;
  struct Cookie *lastc=NULL;
  time_t now = time(NULL);
  bool replace_old = FALSE;
  bool badcookie = FALSE; /* cookies are good by default. mmmmm yummy */

#ifdef CURL_DISABLE_VERBOSE_STRINGS
  (void)data;
#endif

  /* First, alloc and init a new struct for it */
  co = calloc(1, sizeof(struct Cookie));
  if(!co)
    return NULL; /* bail out if we're this low on memory */

  if(httpheader) {
    /* This line was read off a HTTP-header */
    const char *ptr;
    const char *semiptr;
    char *what;

    what = malloc(MAX_COOKIE_LINE);
    if(!what) {
      free(co);
      return NULL;
    }

    semiptr=strchr(lineptr, ';'); /* first, find a semicolon */

    while(*lineptr && ISBLANK(*lineptr))
      lineptr++;

    ptr = lineptr;
    do {
      /* we have a <what>=<this> pair or a stand-alone word here */
      name[0]=what[0]=0; /* init the buffers */
      if(1 <= sscanf(ptr, "%" MAX_NAME_TXT "[^;\r\n =] =%"
                     MAX_COOKIE_LINE_TXT "[^;\r\n]",
                     name, what)) {
        /* Use strstore() below to properly deal with received cookie
           headers that have the same string property set more than once,
           and then we use the last one. */
        const char *whatptr;
        bool done = FALSE;
        bool sep;
        size_t len=strlen(what);
        const char *endofn = &ptr[ strlen(name) ];

        /* skip trailing spaces in name */
        while(*endofn && ISBLANK(*endofn))
          endofn++;

        /* name ends with a '=' ? */
        sep = (*endofn == '=')?TRUE:FALSE;

        /* Strip off trailing whitespace from the 'what' */
        while(len && ISBLANK(what[len-1])) {
          what[len-1]=0;
          len--;
        }

        /* Skip leading whitespace from the 'what' */
        whatptr=what;
        while(*whatptr && ISBLANK(*whatptr))
          whatptr++;

        if(!len) {
          /* this was a "<name>=" with no content, and we must allow
             'secure' and 'httponly' specified this weirdly */
          done = TRUE;
          if(Curl_raw_equal("secure", name))
            co->secure = TRUE;
          else if(Curl_raw_equal("httponly", name))
            co->httponly = TRUE;
          else if(sep)
            /* there was a '=' so we're not done parsing this field */
            done = FALSE;
        }
        if(done)
          ;
        else if(Curl_raw_equal("path", name)) {
          strstore(&co->path, whatptr);
          if(!co->path) {
            badcookie = TRUE; /* out of memory bad */
            break;
          }
          co->spath = sanitize_cookie_path(co->path);
          if(!co->spath) {
            badcookie = TRUE; /* out of memory bad */
            break;
          }
        }
        else if(Curl_raw_equal("domain", name)) {
          bool is_ip;
          const char *dotp;

          /* Now, we make sure that our host is within the given domain,
             or the given domain is not valid and thus cannot be set. */

          if('.' == whatptr[0])
            whatptr++; /* ignore preceding dot */

          is_ip = isip(domain ? domain : whatptr);

          /* check for more dots */
          dotp = strchr(whatptr, '.');
          if(!dotp)
            domain=":";

          if(!domain
             || (is_ip && !strcmp(whatptr, domain))
             || (!is_ip && tailmatch(whatptr, domain))) {
            strstore(&co->domain, whatptr);
            if(!co->domain) {
              badcookie = TRUE;
              break;
            }
            if(!is_ip)
              co->tailmatch=TRUE; /* we always do that if the domain name was
                                     given */
          }
          else {
            /* we did not get a tailmatch and then the attempted set domain
               is not a domain to which the current host belongs. Mark as
               bad. */
            badcookie=TRUE;
            infof(data, "skipped cookie with bad tailmatch domain: %s\n",
                  whatptr);
          }
        }
        else if(Curl_raw_equal("version", name)) {
          strstore(&co->version, whatptr);
          if(!co->version) {
            badcookie = TRUE;
            break;
          }
        }
        else if(Curl_raw_equal("max-age", name)) {
          /* Defined in RFC2109:

             Optional.  The Max-Age attribute defines the lifetime of the
             cookie, in seconds.  The delta-seconds value is a decimal non-
             negative integer.  After delta-seconds seconds elapse, the
             client should discard the cookie.  A value of zero means the
             cookie should be discarded immediately.

          */
          strstore(&co->maxage, whatptr);
          if(!co->maxage) {
            badcookie = TRUE;
            break;
          }
        }
        else if(Curl_raw_equal("expires", name)) {
          strstore(&co->expirestr, whatptr);
          if(!co->expirestr) {
            badcookie = TRUE;
            break;
          }
        }
        else if(!co->name) {
          co->name = strdup(name);
          co->value = strdup(whatptr);
          if(!co->name || !co->value) {
            badcookie = TRUE;
            break;
          }
        }
        /*
          else this is the second (or more) name we don't know
          about! */
      }
      else {
        /* this is an "illegal" <what>=<this> pair */
      }

      if(!semiptr || !*semiptr) {
        /* we already know there are no more cookies */
        semiptr = NULL;
        continue;
      }

      ptr=semiptr+1;
      while(*ptr && ISBLANK(*ptr))
        ptr++;
      semiptr=strchr(ptr, ';'); /* now, find the next semicolon */

      if(!semiptr && *ptr)
        /* There are no more semicolons, but there's a final name=value pair
           coming up */
        semiptr=strchr(ptr, '\0');
    } while(semiptr);

    if(co->maxage) {
      co->expires =
        curlx_strtoofft((*co->maxage=='\"')?
                        &co->maxage[1]:&co->maxage[0], NULL, 10);
      if(CURL_OFF_T_MAX - now < co->expires)
        /* avoid overflow */
        co->expires = CURL_OFF_T_MAX;
      else
        co->expires += now;
    }
    else if(co->expirestr) {
      /* Note that if the date couldn't get parsed for whatever reason,
         the cookie will be treated as a session cookie */
      co->expires = curl_getdate(co->expirestr, NULL);

      /* Session cookies have expires set to 0 so if we get that back
         from the date parser let's add a second to make it a
         non-session cookie */
      if(co->expires == 0)
        co->expires = 1;
      else if(co->expires < 0)
        co->expires = 0;
    }

    if(!badcookie && !co->domain) {
      if(domain) {
        /* no domain was given in the header line, set the default */
        co->domain=strdup(domain);
        if(!co->domain)
          badcookie = TRUE;
      }
    }

    if(!badcookie && !co->path && path) {
      /* No path was given in the header line, set the default.
         Note that the passed-in path to this function MAY have a '?' and
         following part that MUST not be stored as part of the path. */
      char *queryp = strchr(path, '?');

      /* queryp is where the interesting part of the path ends, so now we
         want to the find the last */
      char *endslash;
      if(!queryp)
        endslash = strrchr(path, '/');
      else
        endslash = memrchr(path, '/', (size_t)(queryp - path));
      if(endslash) {
        size_t pathlen = (size_t)(endslash-path+1); /* include ending slash */
        co->path=malloc(pathlen+1); /* one extra for the zero byte */
        if(co->path) {
          memcpy(co->path, path, pathlen);
          co->path[pathlen]=0; /* zero terminate */
          co->spath = sanitize_cookie_path(co->path);
          if(!co->spath)
            badcookie = TRUE; /* out of memory bad */
        }
        else
          badcookie = TRUE;
      }
    }

    free(what);

    if(badcookie || !co->name) {
      /* we didn't get a cookie name or a bad one,
         this is an illegal line, bail out */
      freecookie(co);
      return NULL;
    }

  }
  else {
    /* This line is NOT a HTTP header style line, we do offer support for
       reading the odd netscape cookies-file format here */
    char *ptr;
    char *firstptr;
    char *tok_buf=NULL;
    int fields;

    /* IE introduced HTTP-only cookies to prevent XSS attacks. Cookies
       marked with httpOnly after the domain name are not accessible
       from javascripts, but since curl does not operate at javascript
       level, we include them anyway. In Firefox's cookie files, these
       lines are preceded with #HttpOnly_ and then everything is
       as usual, so we skip 10 characters of the line..
    */
    if(strncmp(lineptr, "#HttpOnly_", 10) == 0) {
      lineptr += 10;
      co->httponly = TRUE;
    }

    if(lineptr[0]=='#') {
      /* don't even try the comments */
      free(co);
      return NULL;
    }
    /* strip off the possible end-of-line characters */
    ptr=strchr(lineptr, '\r');
    if(ptr)
      *ptr=0; /* clear it */
    ptr=strchr(lineptr, '\n');
    if(ptr)
      *ptr=0; /* clear it */

    firstptr=strtok_r(lineptr, "\t", &tok_buf); /* tokenize it on the TAB */

    /* Now loop through the fields and init the struct we already have
       allocated */
    for(ptr=firstptr, fields=0; ptr && !badcookie;
        ptr=strtok_r(NULL, "\t", &tok_buf), fields++) {
      switch(fields) {
      case 0:
        if(ptr[0]=='.') /* skip preceding dots */
          ptr++;
        co->domain = strdup(ptr);
        if(!co->domain)
          badcookie = TRUE;
        break;
      case 1:
        /* This field got its explanation on the 23rd of May 2001 by
           Andrs Garca:

           flag: A TRUE/FALSE value indicating if all machines within a given
           domain can access the variable. This value is set automatically by
           the browser, depending on the value you set for the domain.

           As far as I can see, it is set to true when the cookie says
           .domain.com and to false when the domain is complete www.domain.com
        */
        co->tailmatch = Curl_raw_equal(ptr, "TRUE")?TRUE:FALSE;
        break;
      case 2:
        /* It turns out, that sometimes the file format allows the path
           field to remain not filled in, we try to detect this and work
           around it! Andrs Garca made us aware of this... */
        if(strcmp("TRUE", ptr) && strcmp("FALSE", ptr)) {
          /* only if the path doesn't look like a boolean option! */
          co->path = strdup(ptr);
          if(!co->path)
            badcookie = TRUE;
          else {
            co->spath = sanitize_cookie_path(co->path);
            if(!co->spath) {
              badcookie = TRUE; /* out of memory bad */
            }
          }
          break;
        }
        /* this doesn't look like a path, make one up! */
        co->path = strdup("/");
        if(!co->path)
          badcookie = TRUE;
        co->spath = strdup("/");
        if(!co->spath)
          badcookie = TRUE;
        fields++; /* add a field and fall down to secure */
        /* FALLTHROUGH */
      case 3:
        co->secure = Curl_raw_equal(ptr, "TRUE")?TRUE:FALSE;
        break;
      case 4:
        co->expires = curlx_strtoofft(ptr, NULL, 10);
        break;
      case 5:
        co->name = strdup(ptr);
        if(!co->name)
          badcookie = TRUE;
        break;
      case 6:
        co->value = strdup(ptr);
        if(!co->value)
          badcookie = TRUE;
        break;
      }
    }
    if(6 == fields) {
      /* we got a cookie with blank contents, fix it */
      co->value = strdup("");
      if(!co->value)
        badcookie = TRUE;
      else
        fields++;
    }

    if(!badcookie && (7 != fields))
      /* we did not find the sufficient number of fields */
      badcookie = TRUE;

    if(badcookie) {
      freecookie(co);
      return NULL;
    }

  }

  if(!c->running &&    /* read from a file */
     c->newsession &&  /* clean session cookies */
     !co->expires) {   /* this is a session cookie since it doesn't expire! */
    freecookie(co);
    return NULL;
  }

  co->livecookie = c->running;

  /* now, we have parsed the incoming line, we must now check if this
     superceeds an already existing cookie, which it may if the previous have
     the same domain and path as this */

  /* at first, remove expired cookies */
  remove_expired(c);

  clist = c->cookies;
  replace_old = FALSE;
  while(clist) {
    if(Curl_raw_equal(clist->name, co->name)) {
      /* the names are identical */

      if(clist->domain && co->domain) {
        if(Curl_raw_equal(clist->domain, co->domain))
          /* The domains are identical */
          replace_old=TRUE;
      }
      else if(!clist->domain && !co->domain)
        replace_old = TRUE;

      if(replace_old) {
        /* the domains were identical */

        if(clist->spath && co->spath) {
          if(Curl_raw_equal(clist->spath, co->spath)) {
            replace_old = TRUE;
          }
          else
            replace_old = FALSE;
        }
        else if(!clist->spath && !co->spath)
          replace_old = TRUE;
        else
          replace_old = FALSE;

      }

      if(replace_old && !co->livecookie && clist->livecookie) {
        /* Both cookies matched fine, except that the already present
           cookie is "live", which means it was set from a header, while
           the new one isn't "live" and thus only read from a file. We let
           live cookies stay alive */

        /* Free the newcomer and get out of here! */
        freecookie(co);
        return NULL;
      }

      if(replace_old) {
        co->next = clist->next; /* get the next-pointer first */

        /* then free all the old pointers */
        free(clist->name);
        free(clist->value);
        free(clist->domain);
        free(clist->path);
        free(clist->spath);
        free(clist->expirestr);
        free(clist->version);
        free(clist->maxage);

        *clist = *co;  /* then store all the new data */

        free(co);   /* free the newly alloced memory */
        co = clist; /* point to the previous struct instead */

        /* We have replaced a cookie, now skip the rest of the list but
           make sure the 'lastc' pointer is properly set */
        do {
          lastc = clist;
          clist = clist->next;
        } while(clist);
        break;
      }
    }
    lastc = clist;
    clist = clist->next;
  }

  if(c->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(data, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" CURL_FORMAT_CURL_OFF_T "\n",
          replace_old?"Replaced":"Added", co->name, co->value,
          co->domain, co->path, co->expires);

  if(!replace_old) {
    /* then make the last item point on this new one */
    if(lastc)
      lastc->next = co;
    else
      c->cookies = co;
    c->numcookies++; /* one more cookie in the jar */
  }

  return co;
}