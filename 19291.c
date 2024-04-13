has_option_name (const char *line, const char *name)
{
  const char *s;
  int n = strlen (name);

  s = strstr (line, name);
  return (s && (s == line || spacep (s-1))
          && (!s[n] || spacep (s+n) || s[n] == '=')) ? (s+n) : NULL;
}