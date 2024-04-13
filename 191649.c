void module_register (void)
{
	plugin_register_complex_config ("network", network_config);
	plugin_register_init   ("network", network_init);
	plugin_register_flush   ("network", network_flush,
			/* user_data = */ NULL);
} /* void module_register */