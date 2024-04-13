void vp8_blend_b_c(unsigned char *y, unsigned char *u, unsigned char *v,
                   int y_1, int u_1, int v_1, int alpha, int stride) {
  int i, j;
  int y1_const = y_1 * ((1 << 16) - alpha);
  int u1_const = u_1 * ((1 << 16) - alpha);
  int v1_const = v_1 * ((1 << 16) - alpha);

  for (i = 0; i < 4; ++i) {
    for (j = 0; j < 4; ++j) {
      y[j] = (y[j] * alpha + y1_const) >> 16;
    }
    y += stride;
  }

  stride >>= 1;

  for (i = 0; i < 2; ++i) {
    for (j = 0; j < 2; ++j) {
      u[j] = (u[j] * alpha + u1_const) >> 16;
      v[j] = (v[j] * alpha + v1_const) >> 16;
    }
    u += stride;
    v += stride;
  }
}