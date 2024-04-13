msg_event_raise(const uschar * event, const address_item * addr)
{
const uschar * save_domain = deliver_domain;
uschar * save_local =  deliver_localpart;
const uschar * save_host = deliver_host;
const uschar * save_address = deliver_host_address;
const int      save_port =   deliver_host_port;

if (!addr->transport)
  return;

router_name =    addr->router ? addr->router->name : NULL;
transport_name = addr->transport->name;
deliver_domain = addr->domain;
deliver_localpart = addr->local_part;
deliver_host =   addr->host_used ? addr->host_used->name : NULL;

(void) event_raise(addr->transport->event_action, event,
	  addr->host_used
          || Ustrcmp(addr->transport->driver_name, "smtp") == 0
	  || Ustrcmp(addr->transport->driver_name, "lmtp") == 0
	 ? addr->message : NULL);

deliver_host_port =    save_port;
deliver_host_address = save_address;
deliver_host =      save_host;
deliver_localpart = save_local;
deliver_domain =    save_domain;
router_name = transport_name = NULL;
}