parse_callout_of_name(Node** np, int cterm, UChar** src, UChar* end, ScanEnv* env)
{
  int r;
  int i;
  int in;
  int num;
  int name_id;
  int arg_num;
  int max_arg_num;
  int opt_arg_num;
  int is_not_single;
  OnigCodePoint c;
  UChar* name_start;
  UChar* name_end;
  UChar* tag_start;
  UChar* tag_end;
  Node*  node;
  CalloutListEntry* e;
  RegexExt* ext;
  unsigned int types[ONIG_CALLOUT_MAX_ARGS_NUM];
  OnigValue    vals[ONIG_CALLOUT_MAX_ARGS_NUM];
  OnigEncoding enc = env->enc;
  UChar* p = *src;

  //PFETCH_READY;
  if (PEND) return ONIGERR_INVALID_CALLOUT_PATTERN;

  node = 0;
  name_start = p;
  while (1) {
    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
    name_end = p;
    PFETCH_S(c);
    if (c == cterm || c == '[' || c == '{') break;
  }

  if (! is_allowed_callout_name(enc, name_start, name_end))
    return ONIGERR_INVALID_CALLOUT_NAME;

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

  if (c == '{') {
    UChar* save;

    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;

    /* read for single check only */
    save = p;
    arg_num = parse_callout_args(1, '}', &p, end, 0, 0, env);
    if (arg_num < 0) return arg_num;

    is_not_single = PPEEK_IS(cterm) ?  0 : 1;
    p = save;
    r = get_callout_name_id_by_name(enc, is_not_single, name_start, name_end,
                                    &name_id);
    if (r != ONIG_NORMAL) return r;

    max_arg_num = get_callout_arg_num_by_name_id(name_id);
    for (i = 0; i < max_arg_num; i++) {
      types[i] = get_callout_arg_type_by_name_id(name_id, i);
    }

    arg_num = parse_callout_args(0, '}', &p, end, types, vals, env);
    if (arg_num < 0) return arg_num;

    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
    PFETCH_S(c);
  }
  else {
    arg_num = 0;

    is_not_single = 0;
    r = get_callout_name_id_by_name(enc, is_not_single, name_start, name_end,
                                      &name_id);
    if (r != ONIG_NORMAL) return r;

    max_arg_num = get_callout_arg_num_by_name_id(name_id);
    for (i = 0; i < max_arg_num; i++) {
      types[i] = get_callout_arg_type_by_name_id(name_id, i);
    }
  }

  in = onig_get_callout_in_by_name_id(name_id);
  opt_arg_num = get_callout_opt_arg_num_by_name_id(name_id);
  if (arg_num > max_arg_num || arg_num < (max_arg_num - opt_arg_num))
    return ONIGERR_INVALID_CALLOUT_ARG;

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

  r = node_new_callout(&node, ONIG_CALLOUT_OF_NAME, num, name_id, env);
  if (r != ONIG_NORMAL) return r;

  e = onig_reg_callout_list_at(env->reg, num);
  e->of         = ONIG_CALLOUT_OF_NAME;
  e->in         = in;
  e->name_id    = name_id;
  e->type       = onig_get_callout_type_by_name_id(name_id);
  e->start_func = onig_get_callout_start_func_by_name_id(name_id);
  e->end_func   = onig_get_callout_end_func_by_name_id(name_id);
  e->u.arg.num        = max_arg_num;
  e->u.arg.passed_num = arg_num;
  for (i = 0; i < max_arg_num; i++) {
    e->u.arg.types[i] = types[i];
    if (i < arg_num)
      e->u.arg.vals[i] = vals[i];
    else
      e->u.arg.vals[i] = get_callout_opt_default_by_name_id(name_id, i);
  }

  *np = node;
  *src = p;
  return 0;
}