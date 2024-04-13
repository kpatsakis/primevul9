clear_optimize_info(regex_t* reg)
{
  reg->optimize      = OPTIMIZE_NONE;
  reg->anchor        = 0;
  reg->anc_dist_min  = 0;
  reg->anc_dist_max  = 0;
  reg->sub_anchor    = 0;
  reg->exact_end     = (UChar* )NULL;
  reg->map_offset    = 0;
  reg->threshold_len = 0;
  if (IS_NOT_NULL(reg->exact)) {
    xfree(reg->exact);
    reg->exact = (UChar* )NULL;
  }
}