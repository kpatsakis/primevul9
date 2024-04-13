static __init void mce_init_banks(void)
{
	int i;

	for (i = 0; i < mca_cfg.banks; i++) {
		struct mce_bank *b = &mce_banks[i];
		struct device_attribute *a = &b->attr;

		sysfs_attr_init(&a->attr);
		a->attr.name	= b->attrname;
		snprintf(b->attrname, ATTR_LEN, "bank%d", i);

		a->attr.mode	= 0644;
		a->show		= show_bank;
		a->store	= set_bank;
	}
}