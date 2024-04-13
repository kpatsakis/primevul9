host_is_tls_on_connect_port(int port)
{
int sep = 0;
const uschar * list = tls_in.on_connect_ports;

if (tls_in.on_connect) return TRUE;

for (uschar * s, * end; s = string_nextinlist(&list, &sep, NULL, 0); )
  if (Ustrtol(s, &end, 10) == port)
    return TRUE;

return FALSE;
}