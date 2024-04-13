parse_callout_of_contents(Node** np, int cterm, UChar** src, UChar* end, ScanEnv* env)
{
  int r;
  int i;
  int in;
  int num;
  OnigCodePoint c;
  UChar* code_start;
  UChar* code_end;
  UChar* contents;
  UChar* tag_start;
  UChar* tag_end;
  int brace_nest;
  CalloutListEntry* e;
  RegexExt* ext;
  OnigEncoding enc = env->enc;
  UChar* p = *src;

  if (PEND) return ONIGERR_INVALID_CALLOUT_PATTERN;

  brace_nest = 0;
  while (PPEEK_IS('{')) {
    brace_nest++;
    PINC_S;
    if (PEND) return ONIGERR_INVALID_CALLOUT_PATTERN;
  }

  in = ONIG_CALLOUT_IN_PROGRESS;
  code_start = p;
  while (1) {
    if (PEND) return ONIGERR_INVALID_CALLOUT_PATTERN;

    code_end = p;
    PFETCH_S(c);
    if (c == '}') {
      i = brace_nest;
      while (i > 0) {
        if (PEND) return ONIGERR_INVALID_CALLOUT_PATTERN;
        PFETCH_S(c);
        if (c == '}') i--;
        else break;
      }
      if (i == 0) break;
    }
  }

  if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;

  PFETCH_S(c);
  if (c == '[') {
    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
    tag_start = p;
    while (! PEND) {
      if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
      tag_end = p;
      PFETCH_S(c);
      if (c == ']') break;
    }
    if (! is_allowed_callout_tag_name(enc, tag_start, tag_end))
      return ONIGERR_INVALID_CALLOUT_TAG_NAME;

    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
    PFETCH_S(c);
  }
  else {
    tag_start = tag_end = 0;
  }

  if (c == 'X') {
    in |= ONIG_CALLOUT_IN_RETRACTION;
    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
    PFETCH_S(c);
  }
  else if (c == '<') {
    in = ONIG_CALLOUT_IN_RETRACTION;
    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
    PFETCH_S(c);
  }
  else if (c == '>') { /* no needs (default) */
    //in = ONIG_CALLOUT_IN_PROGRESS;
    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
    PFETCH_S(c);
  }

  if (c != cterm)
    return ONIGERR_INVALID_CALLOUT_PATTERN;

  r = reg_callout_list_entry(env, &num);
  if (r != 0) return r;

  ext = onig_get_regex_ext(env->reg);
  if (IS_NULL(ext->pattern)) {
    r = onig_ext_set_pattern(env->reg, env->pattern, env->pattern_end);
    if (r != ONIG_NORMAL) return r;
  }

  if (tag_start != tag_end) {
    r = callout_tag_entry(env->reg, tag_start, tag_end, num);
    if (r != ONIG_NORMAL) return r;
  }

  contents = onigenc_strdup(enc, code_start, code_end);
  CHECK_NULL_RETURN_MEMERR(contents);

  r = node_new_callout(np, ONIG_CALLOUT_OF_CONTENTS, num, ONIG_NON_NAME_ID, env);
  if (r != 0) {
    xfree(contents);
    return r;
  }

  e = onig_reg_callout_list_at(env->reg, num);
  e->of      = ONIG_CALLOUT_OF_CONTENTS;
  e->in      = in;
  e->name_id = ONIG_NON_NAME_ID;
  e->u.content.start = contents;
  e->u.content.end   = contents + (code_end - code_start);

  *src = p;
  return 0;
}