int mwifiex_stop_bg_scan(struct mwifiex_private *priv)
{
	struct mwifiex_bg_scan_cfg *bgscan_cfg;

	if (!priv->sched_scanning) {
		dev_dbg(priv->adapter->dev, "bgscan already stopped!\n");
		return 0;
	}

	bgscan_cfg = kzalloc(sizeof(*bgscan_cfg), GFP_KERNEL);
	if (!bgscan_cfg)
		return -ENOMEM;

	bgscan_cfg->bss_type = MWIFIEX_BSS_MODE_INFRA;
	bgscan_cfg->action = MWIFIEX_BGSCAN_ACT_SET;
	bgscan_cfg->enable = false;

	if (mwifiex_send_cmd(priv, HostCmd_CMD_802_11_BG_SCAN_CONFIG,
			     HostCmd_ACT_GEN_SET, 0, bgscan_cfg, true)) {
		kfree(bgscan_cfg);
		return -EFAULT;
	}

	kfree(bgscan_cfg);
	priv->sched_scanning = false;

	return 0;
}