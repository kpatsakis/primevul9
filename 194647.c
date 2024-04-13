add_host_info_for_log(gstring * g)
{
if (sender_fullhost)
  {
  if (LOGGING(dnssec) && sender_host_dnssec)	/*XXX sender_helo_dnssec? */
    g = string_catn(g, US" DS", 3);
  g = string_append(g, 2, US" H=", sender_fullhost);
  if (LOGGING(incoming_interface) && interface_address != NULL)
    {
    g = string_cat(g,
      string_sprintf(" I=[%s]:%d", interface_address, interface_port));
    }
  }
if (tcp_in_fastopen && !tcp_in_fastopen_logged)
  {
  g = string_catn(g, US" TFO", 4);
  tcp_in_fastopen_logged = TRUE;
  }
if (sender_ident)
  g = string_append(g, 2, US" U=", sender_ident);
if (received_protocol)
  g = string_append(g, 2, US" P=", received_protocol);
return g;
}