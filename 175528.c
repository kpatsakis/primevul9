static void vp8_de_mblock(YV12_BUFFER_CONFIG *post, int q) {
  vpx_mbpost_proc_across_ip(post->y_buffer, post->y_stride, post->y_height,
                            post->y_width, q2mbl(q));
  vpx_mbpost_proc_down(post->y_buffer, post->y_stride, post->y_height,
                       post->y_width, q2mbl(q));
}