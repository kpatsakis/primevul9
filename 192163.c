void mce_disable_bank(int bank)
{
	if (bank >= mca_cfg.banks) {
		pr_warn(FW_BUG
			"Ignoring request to disable invalid MCA bank %d.\n",
			bank);
		return;
	}
	set_bit(bank, mce_banks_ce_disabled);
	on_each_cpu(__mce_disable_bank, &bank, 1);
}