concat_opt_exact(OptStr* to, OptStr* add, OnigEncoding enc)
{
  int i, j, len, r;
  UChar *p, *end;
  OptAnc tanc;

  r = 0;
  p = add->s;
  end = p + add->len;
  for (i = to->len; p < end; ) {
    len = enclen(enc, p);
    if (i + len > OPT_EXACT_MAXLEN) {
      r = 1; /* 1:full */
      break;
    }
    for (j = 0; j < len && p < end; j++)
      to->s[i++] = *p++;
  }

  to->len = i;
  to->reach_end = (p == end ? add->reach_end : 0);

  concat_opt_anc_info(&tanc, &to->anc, &add->anc, 1, 1);
  if (! to->reach_end) tanc.right = 0;
  copy_opt_anc_info(&to->anc, &tanc);

  return r;
}