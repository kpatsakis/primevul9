void vp8_de_noise(VP8_COMMON *cm, YV12_BUFFER_CONFIG *source,
                  YV12_BUFFER_CONFIG *post, int q, int low_var_thresh, int flag,
                  int uvfilter) {
  int mbr;
  double level = 6.0e-05 * q * q * q - .0067 * q * q + .306 * q + .0065;
  int ppl = (int)(level + .5);
  int mb_rows = cm->mb_rows;
  int mb_cols = cm->mb_cols;
  unsigned char *limits = cm->pp_limits_buffer;
  (void)post;
  (void)low_var_thresh;
  (void)flag;

  memset(limits, (unsigned char)ppl, 16 * mb_cols);

  /* TODO: The original code don't filter the 2 outer rows and columns. */
  for (mbr = 0; mbr < mb_rows; ++mbr) {
    vpx_post_proc_down_and_across_mb_row(
        source->y_buffer + 16 * mbr * source->y_stride,
        source->y_buffer + 16 * mbr * source->y_stride, source->y_stride,
        source->y_stride, source->y_width, limits, 16);
    if (uvfilter == 1) {
      vpx_post_proc_down_and_across_mb_row(
          source->u_buffer + 8 * mbr * source->uv_stride,
          source->u_buffer + 8 * mbr * source->uv_stride, source->uv_stride,
          source->uv_stride, source->uv_width, limits, 8);
      vpx_post_proc_down_and_across_mb_row(
          source->v_buffer + 8 * mbr * source->uv_stride,
          source->v_buffer + 8 * mbr * source->uv_stride, source->uv_stride,
          source->uv_stride, source->uv_width, limits, 8);
    }
  }
}