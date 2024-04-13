  const char* Parser::re_attr_insensitive_close(const char* src)
  {
    return sequence < insensitive<'i'>, re_attr_sensitive_close >(src);
  }