onig_initialize(OnigEncoding encodings[], int n)
{
  int i;
  int r;

  if (onig_inited != 0)
    return 0;

  onigenc_init();

  onig_inited = 1;

  for (i = 0; i < n; i++) {
    OnigEncoding enc = encodings[i];
    r = onig_initialize_encoding(enc);
    if (r != 0)
      return r;
  }

  return ONIG_NORMAL;
}