is_in_string_pool(regex_t* reg, UChar* s)
{
  return (s >= reg->string_pool && s < reg->string_pool_end);
}