bool i40e_dcb_need_reconfig(struct i40e_pf *pf,
			    struct i40e_dcbx_config *old_cfg,
			    struct i40e_dcbx_config *new_cfg)
{
	bool need_reconfig = false;

	/* Check if ETS configuration has changed */
	if (memcmp(&new_cfg->etscfg,
		   &old_cfg->etscfg,
		   sizeof(new_cfg->etscfg))) {
		/* If Priority Table has changed reconfig is needed */
		if (memcmp(&new_cfg->etscfg.prioritytable,
			   &old_cfg->etscfg.prioritytable,
			   sizeof(new_cfg->etscfg.prioritytable))) {
			need_reconfig = true;
			dev_dbg(&pf->pdev->dev, "ETS UP2TC changed.\n");
		}

		if (memcmp(&new_cfg->etscfg.tcbwtable,
			   &old_cfg->etscfg.tcbwtable,
			   sizeof(new_cfg->etscfg.tcbwtable)))
			dev_dbg(&pf->pdev->dev, "ETS TC BW Table changed.\n");

		if (memcmp(&new_cfg->etscfg.tsatable,
			   &old_cfg->etscfg.tsatable,
			   sizeof(new_cfg->etscfg.tsatable)))
			dev_dbg(&pf->pdev->dev, "ETS TSA Table changed.\n");
	}

	/* Check if PFC configuration has changed */
	if (memcmp(&new_cfg->pfc,
		   &old_cfg->pfc,
		   sizeof(new_cfg->pfc))) {
		need_reconfig = true;
		dev_dbg(&pf->pdev->dev, "PFC config change detected.\n");
	}

	/* Check if APP Table has changed */
	if (memcmp(&new_cfg->app,
		   &old_cfg->app,
		   sizeof(new_cfg->app))) {
		need_reconfig = true;
		dev_dbg(&pf->pdev->dev, "APP Table change detected.\n");
	}

	dev_dbg(&pf->pdev->dev, "dcb need_reconfig=%d\n", need_reconfig);
	return need_reconfig;
}