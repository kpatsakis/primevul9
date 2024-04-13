int vp8_post_proc_frame(VP8_COMMON *oci, YV12_BUFFER_CONFIG *dest,
                        vp8_ppflags_t *ppflags) {
  int q = oci->filter_level * 10 / 6;
  int flags = ppflags->post_proc_flag;
  int deblock_level = ppflags->deblocking_level;
  int noise_level = ppflags->noise_level;

  if (!oci->frame_to_show) return -1;

  if (q > 63) q = 63;

  if (!flags) {
    *dest = *oci->frame_to_show;

    /* handle problem with extending borders */
    dest->y_width = oci->Width;
    dest->y_height = oci->Height;
    dest->uv_height = dest->y_height / 2;
    oci->postproc_state.last_base_qindex = oci->base_qindex;
    oci->postproc_state.last_frame_valid = 1;
    return 0;
  }
  if (flags & VP8D_ADDNOISE) {
    if (!oci->postproc_state.generated_noise) {
      oci->postproc_state.generated_noise = vpx_calloc(
          oci->Width + 256, sizeof(*oci->postproc_state.generated_noise));
      if (!oci->postproc_state.generated_noise) return 1;
    }
  }

  /* Allocate post_proc_buffer_int if needed */
  if ((flags & VP8D_MFQE) && !oci->post_proc_buffer_int_used) {
    if ((flags & VP8D_DEBLOCK) || (flags & VP8D_DEMACROBLOCK)) {
      int width = (oci->Width + 15) & ~15;
      int height = (oci->Height + 15) & ~15;

      if (vp8_yv12_alloc_frame_buffer(&oci->post_proc_buffer_int, width, height,
                                      VP8BORDERINPIXELS)) {
        vpx_internal_error(&oci->error, VPX_CODEC_MEM_ERROR,
                           "Failed to allocate MFQE framebuffer");
      }

      oci->post_proc_buffer_int_used = 1;

      /* insure that postproc is set to all 0's so that post proc
       * doesn't pull random data in from edge
       */
      memset((&oci->post_proc_buffer_int)->buffer_alloc, 128,
             (&oci->post_proc_buffer)->frame_size);
    }
  }

  vpx_clear_system_state();

  if ((flags & VP8D_MFQE) && oci->postproc_state.last_frame_valid &&
      oci->current_video_frame >= 2 &&
      oci->postproc_state.last_base_qindex < 60 &&
      oci->base_qindex - oci->postproc_state.last_base_qindex >= 20) {
    vp8_multiframe_quality_enhance(oci);
    if (((flags & VP8D_DEBLOCK) || (flags & VP8D_DEMACROBLOCK)) &&
        oci->post_proc_buffer_int_used) {
      vp8_yv12_copy_frame(&oci->post_proc_buffer, &oci->post_proc_buffer_int);
      if (flags & VP8D_DEMACROBLOCK) {
        vp8_deblock(oci, &oci->post_proc_buffer_int, &oci->post_proc_buffer,
                    q + (deblock_level - 5) * 10, 1, 0);
        vp8_de_mblock(&oci->post_proc_buffer, q + (deblock_level - 5) * 10);
      } else if (flags & VP8D_DEBLOCK) {
        vp8_deblock(oci, &oci->post_proc_buffer_int, &oci->post_proc_buffer, q,
                    1, 0);
      }
    }
    /* Move partially towards the base q of the previous frame */
    oci->postproc_state.last_base_qindex =
        (3 * oci->postproc_state.last_base_qindex + oci->base_qindex) >> 2;
  } else if (flags & VP8D_DEMACROBLOCK) {
    vp8_deblock(oci, oci->frame_to_show, &oci->post_proc_buffer,
                q + (deblock_level - 5) * 10, 1, 0);
    vp8_de_mblock(&oci->post_proc_buffer, q + (deblock_level - 5) * 10);

    oci->postproc_state.last_base_qindex = oci->base_qindex;
  } else if (flags & VP8D_DEBLOCK) {
    vp8_deblock(oci, oci->frame_to_show, &oci->post_proc_buffer, q, 1, 0);
    oci->postproc_state.last_base_qindex = oci->base_qindex;
  } else {
    vp8_yv12_copy_frame(oci->frame_to_show, &oci->post_proc_buffer);
    oci->postproc_state.last_base_qindex = oci->base_qindex;
  }
  oci->postproc_state.last_frame_valid = 1;

  if (flags & VP8D_ADDNOISE) {
    if (oci->postproc_state.last_q != q ||
        oci->postproc_state.last_noise != noise_level) {
      double sigma;
      struct postproc_state *ppstate = &oci->postproc_state;
      vpx_clear_system_state();
      sigma = noise_level + .5 + .6 * q / 63.0;
      ppstate->clamp =
          vpx_setup_noise(sigma, ppstate->generated_noise, oci->Width + 256);
      ppstate->last_q = q;
      ppstate->last_noise = noise_level;
    }

    vpx_plane_add_noise(
        oci->post_proc_buffer.y_buffer, oci->postproc_state.generated_noise,
        oci->postproc_state.clamp, oci->postproc_state.clamp,
        oci->post_proc_buffer.y_width, oci->post_proc_buffer.y_height,
        oci->post_proc_buffer.y_stride);
  }

  *dest = oci->post_proc_buffer;

  /* handle problem with extending borders */
  dest->y_width = oci->Width;
  dest->y_height = oci->Height;
  dest->uv_height = dest->y_height / 2;
  return 0;
}