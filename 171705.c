necp6_print_page(gx_device_printer *pdev, gp_file *prn_stream)
{
  char necp6_init_string [] = "\033@\033P\033l\000\r\034\063\001\033Q";

  return dot24_print_page(pdev, prn_stream, necp6_init_string, sizeof(necp6_init_string));
}