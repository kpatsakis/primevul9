tokenize_string(memarea_t *area,
                const char *start, const char *end, smartlist_t *out,
                token_rule_t *table, int flags)
{
  const char **s;
  directory_token_t *tok = NULL;
  int counts[_NIL];
  int i;
  int first_nonannotation;
  int prev_len = smartlist_len(out);
  tor_assert(area);

  s = &start;
  if (!end)
    end = start+strlen(start);
  for (i = 0; i < _NIL; ++i)
    counts[i] = 0;

  SMARTLIST_FOREACH(out, const directory_token_t *, t, ++counts[t->tp]);

  while (*s < end && (!tok || tok->tp != _EOF)) {
    tok = get_next_token(area, s, end, table);
    if (tok->tp == _ERR) {
      log_warn(LD_DIR, "parse error: %s", tok->error);
      token_clear(tok);
      return -1;
    }
    ++counts[tok->tp];
    smartlist_add(out, tok);
    *s = eat_whitespace_eos(*s, end);
  }

  if (flags & TS_NOCHECK)
    return 0;

  if ((flags & TS_ANNOTATIONS_OK)) {
    first_nonannotation = -1;
    for (i = 0; i < smartlist_len(out); ++i) {
      tok = smartlist_get(out, i);
      if (tok->tp < MIN_ANNOTATION || tok->tp > MAX_ANNOTATION) {
        first_nonannotation = i;
        break;
      }
    }
    if (first_nonannotation < 0) {
      log_warn(LD_DIR, "parse error: item contains only annotations");
      return -1;
    }
    for (i=first_nonannotation;  i < smartlist_len(out); ++i) {
      tok = smartlist_get(out, i);
      if (tok->tp >= MIN_ANNOTATION && tok->tp <= MAX_ANNOTATION) {
        log_warn(LD_DIR, "parse error: Annotations mixed with keywords");
        return -1;
      }
    }
    if ((flags & TS_NO_NEW_ANNOTATIONS)) {
      if (first_nonannotation != prev_len) {
        log_warn(LD_DIR, "parse error: Unexpected annotations.");
        return -1;
      }
    }
  } else {
    for (i=0;  i < smartlist_len(out); ++i) {
      tok = smartlist_get(out, i);
      if (tok->tp >= MIN_ANNOTATION && tok->tp <= MAX_ANNOTATION) {
        log_warn(LD_DIR, "parse error: no annotations allowed.");
        return -1;
      }
    }
    first_nonannotation = 0;
  }
  for (i = 0; table[i].t; ++i) {
    if (counts[table[i].v] < table[i].min_cnt) {
      log_warn(LD_DIR, "Parse error: missing %s element.", table[i].t);
      return -1;
    }
    if (counts[table[i].v] > table[i].max_cnt) {
      log_warn(LD_DIR, "Parse error: too many %s elements.", table[i].t);
      return -1;
    }
    if (table[i].pos & AT_START) {
      if (smartlist_len(out) < 1 ||
          (tok = smartlist_get(out, first_nonannotation))->tp != table[i].v) {
        log_warn(LD_DIR, "Parse error: first item is not %s.", table[i].t);
        return -1;
      }
    }
    if (table[i].pos & AT_END) {
      if (smartlist_len(out) < 1 ||
          (tok = smartlist_get(out, smartlist_len(out)-1))->tp != table[i].v) {
        log_warn(LD_DIR, "Parse error: last item is not %s.", table[i].t);
        return -1;
      }
    }
  }
  return 0;
}