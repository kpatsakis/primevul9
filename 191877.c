dirserv_add_multiple_descriptors(const char *desc, uint8_t purpose,
                                 const char *source,
                                 const char **msg)
{
  was_router_added_t r, r_tmp;
  const char *msg_out;
  smartlist_t *list;
  const char *s;
  int n_parsed = 0;
  time_t now = time(NULL);
  char annotation_buf[ROUTER_ANNOTATION_BUF_LEN];
  char time_buf[ISO_TIME_LEN+1];
  int general = purpose == ROUTER_PURPOSE_GENERAL;
  tor_assert(msg);

  r=ROUTER_ADDED_SUCCESSFULLY; /*Least severe return value. */

  format_iso_time(time_buf, now);
  if (tor_snprintf(annotation_buf, sizeof(annotation_buf),
                   "@uploaded-at %s\n"
                   "@source %s\n"
                   "%s%s%s", time_buf, escaped(source),
                   !general ? "@purpose " : "",
                   !general ? router_purpose_to_string(purpose) : "",
                   !general ? "\n" : "")<0) {
    *msg = "Couldn't format annotations";
    return -1;
  }

  s = desc;
  list = smartlist_new();
  if (!router_parse_list_from_string(&s, NULL, list, SAVED_NOWHERE, 0, 0,
                                     annotation_buf, NULL)) {
    SMARTLIST_FOREACH(list, routerinfo_t *, ri, {
        msg_out = NULL;
        tor_assert(ri->purpose == purpose);
        r_tmp = dirserv_add_descriptor(ri, &msg_out, source);
        if (WRA_MORE_SEVERE(r_tmp, r)) {
          r = r_tmp;
          *msg = msg_out;
        }
      });
  }
  n_parsed += smartlist_len(list);
  smartlist_clear(list);

  s = desc;
  if (!router_parse_list_from_string(&s, NULL, list, SAVED_NOWHERE, 1, 0,
                                     NULL, NULL)) {
    SMARTLIST_FOREACH(list, extrainfo_t *, ei, {
        msg_out = NULL;

        r_tmp = dirserv_add_extrainfo(ei, &msg_out);
        if (WRA_MORE_SEVERE(r_tmp, r)) {
          r = r_tmp;
          *msg = msg_out;
        }
      });
  }
  n_parsed += smartlist_len(list);
  smartlist_free(list);

  if (! *msg) {
    if (!n_parsed) {
      *msg = "No descriptors found in your POST.";
      if (WRA_WAS_ADDED(r))
        r = ROUTER_IS_ALREADY_KNOWN;
    } else {
      *msg = "(no message)";
    }
  }

  return r;
}