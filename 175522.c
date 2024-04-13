void vp8_blend_mb_outer_c(unsigned char *y, unsigned char *u, unsigned char *v,
                          int y_1, int u_1, int v_1, int alpha, int stride) {
  int i, j;
  int y1_const = y_1 * ((1 << 16) - alpha);
  int u1_const = u_1 * ((1 << 16) - alpha);
  int v1_const = v_1 * ((1 << 16) - alpha);

  for (i = 0; i < 2; ++i) {
    for (j = 0; j < 16; ++j) {
      y[j] = (y[j] * alpha + y1_const) >> 16;
    }
    y += stride;
  }

  for (i = 0; i < 12; ++i) {
    y[0] = (y[0] * alpha + y1_const) >> 16;
    y[1] = (y[1] * alpha + y1_const) >> 16;
    y[14] = (y[14] * alpha + y1_const) >> 16;
    y[15] = (y[15] * alpha + y1_const) >> 16;
    y += stride;
  }

  for (i = 0; i < 2; ++i) {
    for (j = 0; j < 16; ++j) {
      y[j] = (y[j] * alpha + y1_const) >> 16;
    }
    y += stride;
  }

  stride >>= 1;

  for (j = 0; j < 8; ++j) {
    u[j] = (u[j] * alpha + u1_const) >> 16;
    v[j] = (v[j] * alpha + v1_const) >> 16;
  }
  u += stride;
  v += stride;

  for (i = 0; i < 6; ++i) {
    u[0] = (u[0] * alpha + u1_const) >> 16;
    v[0] = (v[0] * alpha + v1_const) >> 16;

    u[7] = (u[7] * alpha + u1_const) >> 16;
    v[7] = (v[7] * alpha + v1_const) >> 16;

    u += stride;
    v += stride;
  }

  for (j = 0; j < 8; ++j) {
    u[j] = (u[j] * alpha + u1_const) >> 16;
    v[j] = (v[j] * alpha + v1_const) >> 16;
  }
}