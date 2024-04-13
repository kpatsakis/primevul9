onig_set_callout_of_name(OnigEncoding enc, OnigCalloutType callout_type,
                         UChar* name, UChar* name_end, int in,
                         OnigCalloutFunc start_func,
                         OnigCalloutFunc end_func,
                         int arg_num, unsigned int arg_types[],
                         int opt_arg_num, OnigValue opt_defaults[])
{
  int r;
  int i;
  int j;
  int id;
  int is_not_single;
  CalloutNameEntry* e;
  CalloutNameListEntry* fe;

  if (callout_type != ONIG_CALLOUT_TYPE_SINGLE)
    return ONIGERR_INVALID_ARGUMENT;

  if (arg_num < 0 || arg_num > ONIG_CALLOUT_MAX_ARGS_NUM)
    return ONIGERR_INVALID_CALLOUT_ARG;

  if (opt_arg_num < 0 || opt_arg_num > arg_num)
    return ONIGERR_INVALID_CALLOUT_ARG;

  if (start_func == 0 && end_func == 0)
    return ONIGERR_INVALID_CALLOUT_ARG;

  if ((in & ONIG_CALLOUT_IN_PROGRESS) == 0 && (in & ONIG_CALLOUT_IN_RETRACTION) == 0)
    return ONIGERR_INVALID_CALLOUT_ARG;

  for (i = 0; i < arg_num; i++) {
    unsigned int t = arg_types[i];
    if (t == ONIG_TYPE_VOID)
      return ONIGERR_INVALID_CALLOUT_ARG;
    else {
      if (i >= arg_num - opt_arg_num) {
        if (t != ONIG_TYPE_LONG && t != ONIG_TYPE_CHAR && t != ONIG_TYPE_STRING &&
            t != ONIG_TYPE_TAG)
          return ONIGERR_INVALID_CALLOUT_ARG;
      }
      else {
        if (t != ONIG_TYPE_LONG) {
          t = t & ~ONIG_TYPE_LONG;
          if (t != ONIG_TYPE_CHAR && t != ONIG_TYPE_STRING && t != ONIG_TYPE_TAG)
            return ONIGERR_INVALID_CALLOUT_ARG;
        }
      }
    }
  }

  if (! is_allowed_callout_name(enc, name, name_end)) {
    return ONIGERR_INVALID_CALLOUT_NAME;
  }

  is_not_single = (callout_type != ONIG_CALLOUT_TYPE_SINGLE);
  id = callout_name_entry(&e, enc, is_not_single, name, name_end);
  if (id < 0) return id;

  r = ONIG_NORMAL;
  if (IS_NULL(GlobalCalloutNameList)) {
    r = make_callout_func_list(&GlobalCalloutNameList, 10);
    if (r != ONIG_NORMAL) return r;
  }

  while (id >= GlobalCalloutNameList->n) {
    int rid;
    r = callout_func_list_add(GlobalCalloutNameList, &rid);
    if (r != ONIG_NORMAL) return r;
  }

  fe = GlobalCalloutNameList->v + id;
  fe->type         = callout_type;
  fe->in           = in;
  fe->start_func   = start_func;
  fe->end_func     = end_func;
  fe->arg_num      = arg_num;
  fe->opt_arg_num  = opt_arg_num;
  fe->name         = e->name;

  for (i = 0; i < arg_num; i++) {
    fe->arg_types[i] = arg_types[i];
  }
  for (i = arg_num - opt_arg_num, j = 0; i < arg_num; i++, j++) {
    if (fe->arg_types[i] == ONIG_TYPE_STRING) {
      OnigValue* val = opt_defaults + j;
      UChar* ds = onigenc_strdup(enc, val->s.start, val->s.end);
      CHECK_NULL_RETURN_MEMERR(ds);

      fe->opt_defaults[i].s.start = ds;
      fe->opt_defaults[i].s.end   = ds + (val->s.end - val->s.start);
    }
    else {
      fe->opt_defaults[i] = opt_defaults[j];
    }
  }

  r = id; // return id
  return r;
}