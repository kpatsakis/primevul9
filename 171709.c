dot24_output_run (byte *data, int count, int x_high, gp_file *prn_stream)
{
  int xcount = count / 3;
  gp_fputc (033, prn_stream);
  gp_fputc ('*', prn_stream);
  gp_fputc ((x_high ? 40 : 39), prn_stream);
  gp_fputc (xcount & 0xff, prn_stream);
  gp_fputc (xcount >> 8, prn_stream);
  gp_fwrite (data, 1, count, prn_stream);
}