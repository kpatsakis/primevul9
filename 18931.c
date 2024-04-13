tor_version_is_obsolete(const char *myversion, const char *versionlist)
{
  tor_version_t mine, other;
  int found_newer = 0, found_older = 0, found_newer_in_series = 0,
    found_any_in_series = 0, r, same;
  version_status_t ret = VS_UNRECOMMENDED;
  smartlist_t *version_sl;

  log_debug(LD_CONFIG,"Checking whether version '%s' is in '%s'",
            myversion, versionlist);

  if (tor_version_parse(myversion, &mine)) {
    log_err(LD_BUG,"I couldn't parse my own version (%s)", myversion);
    tor_assert(0);
  }
  version_sl = smartlist_create();
  smartlist_split_string(version_sl, versionlist, ",", SPLIT_SKIP_SPACE, 0);

  if (!strlen(versionlist)) { /* no authorities cared or agreed */
    ret = VS_EMPTY;
    goto done;
  }

  SMARTLIST_FOREACH(version_sl, const char *, cp, {
    if (!strcmpstart(cp, "Tor "))
      cp += 4;

    if (tor_version_parse(cp, &other)) {
      /* Couldn't parse other; it can't be a match. */
    } else {
      same = tor_version_same_series(&mine, &other);
      if (same)
        found_any_in_series = 1;
      r = tor_version_compare(&mine, &other);
      if (r==0) {
        ret = VS_RECOMMENDED;
        goto done;
      } else if (r<0) {
        found_newer = 1;
        if (same)
          found_newer_in_series = 1;
      } else if (r>0) {
        found_older = 1;
      }
    }
  });

  /* We didn't find the listed version. Is it new or old? */
  if (found_any_in_series && !found_newer_in_series && found_newer) {
    ret = VS_NEW_IN_SERIES;
  } else if (found_newer && !found_older) {
    ret = VS_OLD;
  } else if (found_older && !found_newer) {
    ret = VS_NEW;
  } else {
    ret = VS_UNRECOMMENDED;
  }

 done:
  SMARTLIST_FOREACH(version_sl, char *, version, tor_free(version));
  smartlist_free(version_sl);
  return ret;
}