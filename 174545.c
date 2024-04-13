d_hostlog(gstring * g, address_item * addr)
{
host_item * h = addr->host_used;

g = string_append(g, 2, US" H=", h->name);

if (LOGGING(dnssec) && h->dnssec == DS_YES)
  g = string_catn(g, US" DS", 3);

g = string_append(g, 3, US" [", h->address, US"]");

if (LOGGING(outgoing_port))
  g = string_append(g, 2, US":", string_sprintf("%d", h->port));

#ifdef SUPPORT_SOCKS
if (LOGGING(proxy) && proxy_local_address)
  {
  g = string_append(g, 3, US" PRX=[", proxy_local_address, US"]");
  if (LOGGING(outgoing_port))
    g = string_append(g, 2, US":", string_sprintf("%d", proxy_local_port));
  }
#endif

g = d_log_interface(g);

if (testflag(addr, af_tcp_fastopen))
  g = string_catn(g, US" TFO", 4);

return g;
}