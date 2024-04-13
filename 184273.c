onig_free_body(regex_t* reg)
{
  if (IS_NOT_NULL(reg)) {
    ops_free(reg);
    if (IS_NOT_NULL(reg->string_pool)) {
      xfree(reg->string_pool);
      reg->string_pool_end = reg->string_pool = 0;
    }
    if (IS_NOT_NULL(reg->exact))            xfree(reg->exact);
    if (IS_NOT_NULL(reg->repeat_range))     xfree(reg->repeat_range);
    if (IS_NOT_NULL(reg->extp)) {
      free_regex_ext(reg->extp);
      reg->extp = 0;
    }

    onig_names_free(reg);
  }
}