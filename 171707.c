lq850_print_page(gx_device_printer *pdev, gp_file *prn_stream)
{
  char lq850_init_string [] = "\033@\033P\033l\000\r\033\053\001\033Q";

  return dot24_print_page(pdev, prn_stream, lq850_init_string, sizeof(lq850_init_string));
}