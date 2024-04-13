void vp8_blend_mb_inner_c(unsigned char *y, unsigned char *u, unsigned char *v,
                          int y_1, int u_1, int v_1, int alpha, int stride) {
  int i, j;
  int y1_const = y_1 * ((1 << 16) - alpha);
  int u1_const = u_1 * ((1 << 16) - alpha);
  int v1_const = v_1 * ((1 << 16) - alpha);

  y += 2 * stride + 2;
  for (i = 0; i < 12; ++i) {
    for (j = 0; j < 12; ++j) {
      y[j] = (y[j] * alpha + y1_const) >> 16;
    }
    y += stride;
  }

  stride >>= 1;

  u += stride + 1;
  v += stride + 1;

  for (i = 0; i < 6; ++i) {
    for (j = 0; j < 6; ++j) {
      u[j] = (u[j] * alpha + u1_const) >> 16;
      v[j] = (v[j] * alpha + v1_const) >> 16;
    }
    u += stride;
    v += stride;
  }
}