  const char* Parser::re_attr_sensitive_close(const char* src)
  {
    return alternatives < exactly<']'>, exactly<'/'> >(src);
  }