static int __init i740fb_init(void)
{
#ifndef MODULE
	char *option = NULL;

	if (fb_get_options("i740fb", &option))
		return -ENODEV;
	i740fb_setup(option);
#endif

	return pci_register_driver(&i740fb_driver);
}