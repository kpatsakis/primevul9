host_and_ident(BOOL useflag)
{
if (!sender_fullhost)
  string_format_nt(big_buffer, big_buffer_size, "%s%s", useflag ? "U=" : "",
     sender_ident ? sender_ident : US"unknown");
else
  {
  uschar * flag = useflag ? US"H=" : US"";
  uschar * iface = US"";
  if (LOGGING(incoming_interface) && interface_address)
    iface = string_sprintf(" I=[%s]:%d", interface_address, interface_port);
  if (sender_ident)
    string_format_nt(big_buffer, big_buffer_size, "%s%s%s U=%s",
      flag, sender_fullhost, iface, sender_ident);
  else
    string_format_nt(big_buffer, big_buffer_size, "%s%s%s",
      flag, sender_fullhost, iface);
  }
return big_buffer;
}