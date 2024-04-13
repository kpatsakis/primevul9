static int __init mcheck_enable(char *str)
{
	struct mca_config *cfg = &mca_cfg;

	if (*str == 0) {
		enable_p5_mce();
		return 1;
	}
	if (*str == '=')
		str++;
	if (!strcmp(str, "off"))
		cfg->disabled = true;
	else if (!strcmp(str, "no_cmci"))
		cfg->cmci_disabled = true;
	else if (!strcmp(str, "no_lmce"))
		cfg->lmce_disabled = true;
	else if (!strcmp(str, "dont_log_ce"))
		cfg->dont_log_ce = true;
	else if (!strcmp(str, "ignore_ce"))
		cfg->ignore_ce = true;
	else if (!strcmp(str, "bootlog") || !strcmp(str, "nobootlog"))
		cfg->bootlog = (str[0] == 'b');
	else if (!strcmp(str, "bios_cmci_threshold"))
		cfg->bios_cmci_threshold = true;
	else if (!strcmp(str, "recovery"))
		cfg->recovery = true;
	else if (isdigit(str[0])) {
		if (get_option(&str, &cfg->tolerant) == 2)
			get_option(&str, &(cfg->monarch_timeout));
	} else {
		pr_info("mce argument %s ignored. Please use /sys\n", str);
		return 0;
	}
	return 1;
}