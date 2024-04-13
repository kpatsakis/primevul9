compile_regexps (void)
{
  const char *err;
  int offset;

#define COMPILE(re,pattern,options)                                     \
  do {                                                                  \
    re = pcre_compile ((pattern), (options), &err, &offset, NULL);      \
    if (re == NULL) {                                                   \
      ignore_value (write (2, err, strlen (err)));                      \
      abort ();                                                         \
    }                                                                   \
  } while (0)

  COMPILE (re_first_partition, "^/dev/(?:h|s|v)d.1$", 0);
  COMPILE (re_major_minor, "(\\d+)\\.(\\d+)", 0);
}