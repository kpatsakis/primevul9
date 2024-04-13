void pcan_dump_mem(char *prompt, void *p, int l)
{
	pr_info("%s dumping %s (%d bytes):\n",
		PCAN_USB_DRIVER_NAME, prompt ? prompt : "memory", l);
	print_hex_dump(KERN_INFO, PCAN_USB_DRIVER_NAME " ", DUMP_PREFIX_NONE,
		       DUMP_WIDTH, 1, p, l, false);
}