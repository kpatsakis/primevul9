onig_builtin_error(OnigCalloutArgs* args, void* user_data ARG_UNUSED)
{
  int r;
  int n;
  OnigValue val;

  r = onig_get_arg_by_callout_args(args, 0, 0, &val);
  if (r != ONIG_NORMAL) return r;

  n = (int )val.l;
  if (n >= 0) {
    n = ONIGERR_INVALID_CALLOUT_BODY;
  }

  return n;
}