void __init sun4v_mdesc_init(void)
{
	struct mdesc_handle *hp;
	unsigned long len, real_len, status;

	(void) sun4v_mach_desc(0UL, 0UL, &len);

	printk("MDESC: Size is %lu bytes.\n", len);

	hp = mdesc_alloc(len, &memblock_mdesc_ops);
	if (hp == NULL) {
		prom_printf("MDESC: alloc of %lu bytes failed.\n", len);
		prom_halt();
	}

	status = sun4v_mach_desc(__pa(&hp->mdesc), len, &real_len);
	if (status != HV_EOK || real_len > len) {
		prom_printf("sun4v_mach_desc fails, err(%lu), "
			    "len(%lu), real_len(%lu)\n",
			    status, len, real_len);
		mdesc_free(hp);
		prom_halt();
	}

	cur_mdesc = hp;

	mdesc_adi_init();
	report_platform_properties();
}