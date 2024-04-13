static int register_events(cli_events_t *ev)
{
    unsigned i;
    for (i=0;i<sizeof(bc_events)/sizeof(bc_events[0]);i++) {
	if (cli_event_define(ev, bc_events[i].id, bc_events[i].name, bc_events[i].type,
			     bc_events[i].multiple) == -1)
	    return -1;
    }
    return 0;
}