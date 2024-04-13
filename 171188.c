has_valid_scheme (const char *uri)
{
  const char *p;

  p = uri;

  if (!g_ascii_isalpha (*p))
    return FALSE;

  do {
    p++;
  } while (is_valid_scheme_character (*p));

  return *p == ':';
}