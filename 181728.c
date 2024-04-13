static int _aeli_iter(dictkv* kv, void* ud) {
	RAnalEsilInterrupt* interrupt = kv->u;
	r_cons_printf ("%3x: %s\n", kv->k, interrupt->handler->name ? interrupt->handler->name : _handler_no_name);
	return 0;
}