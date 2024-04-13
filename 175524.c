void vp8_deblock(VP8_COMMON *cm, YV12_BUFFER_CONFIG *source,
                 YV12_BUFFER_CONFIG *post, int q, int low_var_thresh,
                 int flag) {
  double level = 6.0e-05 * q * q * q - .0067 * q * q + .306 * q + .0065;
  int ppl = (int)(level + .5);

  const MODE_INFO *mode_info_context = cm->mi;
  int mbr, mbc;

  /* The pixel thresholds are adjusted according to if or not the macroblock
   * is a skipped block.  */
  unsigned char *ylimits = cm->pp_limits_buffer;
  unsigned char *uvlimits = cm->pp_limits_buffer + 16 * cm->mb_cols;
  (void)low_var_thresh;
  (void)flag;

  if (ppl > 0) {
    for (mbr = 0; mbr < cm->mb_rows; ++mbr) {
      unsigned char *ylptr = ylimits;
      unsigned char *uvlptr = uvlimits;
      for (mbc = 0; mbc < cm->mb_cols; ++mbc) {
        unsigned char mb_ppl;

        if (mode_info_context->mbmi.mb_skip_coeff) {
          mb_ppl = (unsigned char)ppl >> 1;
        } else {
          mb_ppl = (unsigned char)ppl;
        }

        memset(ylptr, mb_ppl, 16);
        memset(uvlptr, mb_ppl, 8);

        ylptr += 16;
        uvlptr += 8;
        mode_info_context++;
      }
      mode_info_context++;

      vpx_post_proc_down_and_across_mb_row(
          source->y_buffer + 16 * mbr * source->y_stride,
          post->y_buffer + 16 * mbr * post->y_stride, source->y_stride,
          post->y_stride, source->y_width, ylimits, 16);

      vpx_post_proc_down_and_across_mb_row(
          source->u_buffer + 8 * mbr * source->uv_stride,
          post->u_buffer + 8 * mbr * post->uv_stride, source->uv_stride,
          post->uv_stride, source->uv_width, uvlimits, 8);
      vpx_post_proc_down_and_across_mb_row(
          source->v_buffer + 8 * mbr * source->uv_stride,
          post->v_buffer + 8 * mbr * post->uv_stride, source->uv_stride,
          post->uv_stride, source->uv_width, uvlimits, 8);
    }
  } else {
    vp8_yv12_copy_frame(source, post);
  }
}