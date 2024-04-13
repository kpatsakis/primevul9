static void i40e_print_features(struct i40e_pf *pf)
{
	struct i40e_hw *hw = &pf->hw;
	char *buf;
	int i;

	buf = kmalloc(INFO_STRING_LEN, GFP_KERNEL);
	if (!buf)
		return;

	i = snprintf(buf, INFO_STRING_LEN, "Features: PF-id[%d]", hw->pf_id);
#ifdef CONFIG_PCI_IOV
	i += snprintf(&buf[i], REMAIN(i), " VFs: %d", pf->num_req_vfs);
#endif
	i += snprintf(&buf[i], REMAIN(i), " VSIs: %d QP: %d",
		      pf->hw.func_caps.num_vsis,
		      pf->vsi[pf->lan_vsi]->num_queue_pairs);
	if (pf->flags & I40E_FLAG_RSS_ENABLED)
		i += snprintf(&buf[i], REMAIN(i), " RSS");
	if (pf->flags & I40E_FLAG_FD_ATR_ENABLED)
		i += snprintf(&buf[i], REMAIN(i), " FD_ATR");
	if (pf->flags & I40E_FLAG_FD_SB_ENABLED) {
		i += snprintf(&buf[i], REMAIN(i), " FD_SB");
		i += snprintf(&buf[i], REMAIN(i), " NTUPLE");
	}
	if (pf->flags & I40E_FLAG_DCB_CAPABLE)
		i += snprintf(&buf[i], REMAIN(i), " DCB");
	i += snprintf(&buf[i], REMAIN(i), " VxLAN");
	i += snprintf(&buf[i], REMAIN(i), " Geneve");
	if (pf->flags & I40E_FLAG_PTP)
		i += snprintf(&buf[i], REMAIN(i), " PTP");
	if (pf->flags & I40E_FLAG_VEB_MODE_ENABLED)
		i += snprintf(&buf[i], REMAIN(i), " VEB");
	else
		i += snprintf(&buf[i], REMAIN(i), " VEPA");

	dev_info(&pf->pdev->dev, "%s\n", buf);
	kfree(buf);
	WARN_ON(i > INFO_STRING_LEN);
}