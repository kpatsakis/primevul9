static void __exit i740fb_exit(void)
{
	pci_unregister_driver(&i740fb_driver);
}