cleanup_seccomp (void *p)
{
  scmp_filter_ctx *pp = (scmp_filter_ctx *) p;

  if (*pp)
    seccomp_release (*pp);
}