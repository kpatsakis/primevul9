is_valid_scheme_character (char c)
{
  return g_ascii_isalnum (c) || c == '+' || c == '-' || c == '.';
}