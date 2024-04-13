static double htond (double val) /* {{{ */
{
  static int config = 0;

  union { uint8_t byte[8]; double floating; } in;
  union { uint8_t byte[8]; double floating; } out;

  if (config == 0)
  {
    double d = 8.642135e130;
    uint8_t c[8];

    memcpy (c, &d, 8);

    if ((c[0] == 0x2f) && (c[1] == 0x25)
        && (c[2] == 0xc0) && (c[3] == 0xc7)
        && (c[4] == 0x43) && (c[5] == 0x2b)
        && (c[6] == 0x1f) && (c[7] == 0x5b))
      config = 1; /* need nothing */
    else if ((c[7] == 0x2f) && (c[6] == 0x25)
        && (c[5] == 0xc0) && (c[4] == 0xc7)
        && (c[3] == 0x43) && (c[2] == 0x2b)
        && (c[1] == 0x1f) && (c[0] == 0x5b))
      config = 2; /* endian flip */
    else if ((c[4] == 0x2f) && (c[5] == 0x25)
        && (c[6] == 0xc0) && (c[7] == 0xc7)
        && (c[0] == 0x43) && (c[1] == 0x2b)
        && (c[2] == 0x1f) && (c[3] == 0x5b))
      config = 3; /* int swap */
    else
      config = 4;
  }

  if (isnan (val))
  {
    out.byte[0] = out.byte[1] = out.byte[2] = out.byte[3] = 0x00;
    out.byte[4] = out.byte[5] = 0x00;
    out.byte[6] = 0xf8;
    out.byte[7] = 0x7f;
    return (out.floating);
  }
  else if (config == 1)
    return (val);
  else if (config == 2)
  {
    in.floating = val;
    out.byte[0] = in.byte[7];
    out.byte[1] = in.byte[6];
    out.byte[2] = in.byte[5];
    out.byte[3] = in.byte[4];
    out.byte[4] = in.byte[3];
    out.byte[5] = in.byte[2];
    out.byte[6] = in.byte[1];
    out.byte[7] = in.byte[0];
    return (out.floating);
  }
  else if (config == 3)
  {
    in.floating = val;
    out.byte[0] = in.byte[4];
    out.byte[1] = in.byte[5];
    out.byte[2] = in.byte[6];
    out.byte[3] = in.byte[7];
    out.byte[4] = in.byte[0];
    out.byte[5] = in.byte[1];
    out.byte[6] = in.byte[2];
    out.byte[7] = in.byte[3];
    return (out.floating);
  }
  else
  {
    /* If in doubt, just copy the value back to the caller. */
    return (val);
  }
} /* }}} double htond */