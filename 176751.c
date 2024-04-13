static int sst_scan_uuid_seqno(const char *str, wsrep_uuid_t *uuid,
                               wsrep_seqno_t *seqno) {
  int offt = wsrep_uuid_scan(str, strlen(str), uuid);
  if (offt > 0 && strlen(str) > (unsigned int)offt && ':' == str[offt]) {
    *seqno = strtoll(str + offt + 1, NULL, 10);
    if (*seqno != LLONG_MAX || errno != ERANGE) {
      return 0;
    }
  }

  WSREP_ERROR("Failed to parse uuid:seqno pair: '%s'", str);
  return -EINVAL;
}