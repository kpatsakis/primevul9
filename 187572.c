static struct domain_device *sas_ex_discover_end_dev(
	struct domain_device *parent, int phy_id)
{
	struct expander_device *parent_ex = &parent->ex_dev;
	struct ex_phy *phy = &parent_ex->ex_phy[phy_id];
	struct domain_device *child = NULL;
	struct sas_rphy *rphy;
	int res;

	if (phy->attached_sata_host || phy->attached_sata_ps)
		return NULL;

	child = sas_alloc_device();
	if (!child)
		return NULL;

	kref_get(&parent->kref);
	child->parent = parent;
	child->port   = parent->port;
	child->iproto = phy->attached_iproto;
	memcpy(child->sas_addr, phy->attached_sas_addr, SAS_ADDR_SIZE);
	sas_hash_addr(child->hashed_sas_addr, child->sas_addr);
	if (!phy->port) {
		phy->port = sas_port_alloc(&parent->rphy->dev, phy_id);
		if (unlikely(!phy->port))
			goto out_err;
		if (unlikely(sas_port_add(phy->port) != 0)) {
			sas_port_free(phy->port);
			goto out_err;
		}
	}
	sas_ex_get_linkrate(parent, child, phy);
	sas_device_set_phy(child, phy->port);

#ifdef CONFIG_SCSI_SAS_ATA
	if ((phy->attached_tproto & SAS_PROTOCOL_STP) || phy->attached_sata_dev) {
		if (child->linkrate > parent->min_linkrate) {
			struct sas_phy *cphy = child->phy;
			enum sas_linkrate min_prate = cphy->minimum_linkrate,
				parent_min_lrate = parent->min_linkrate,
				min_linkrate = (min_prate > parent_min_lrate) ?
					       parent_min_lrate : 0;
			struct sas_phy_linkrates rates = {
				.maximum_linkrate = parent->min_linkrate,
				.minimum_linkrate = min_linkrate,
			};
			int ret;

			pr_notice("ex %016llx phy%02d SATA device linkrate > min pathway connection rate, attempting to lower device linkrate\n",
				   SAS_ADDR(child->sas_addr), phy_id);
			ret = sas_smp_phy_control(parent, phy_id,
						  PHY_FUNC_LINK_RESET, &rates);
			if (ret) {
				pr_err("ex %016llx phy%02d SATA device could not set linkrate (%d)\n",
				       SAS_ADDR(child->sas_addr), phy_id, ret);
				goto out_free;
			}
			pr_notice("ex %016llx phy%02d SATA device set linkrate successfully\n",
				  SAS_ADDR(child->sas_addr), phy_id);
			child->linkrate = child->min_linkrate;
		}
		res = sas_get_ata_info(child, phy);
		if (res)
			goto out_free;

		sas_init_dev(child);
		res = sas_ata_init(child);
		if (res)
			goto out_free;
		rphy = sas_end_device_alloc(phy->port);
		if (!rphy)
			goto out_free;
		rphy->identify.phy_identifier = phy_id;

		child->rphy = rphy;
		get_device(&rphy->dev);

		list_add_tail(&child->disco_list_node, &parent->port->disco_list);

		res = sas_discover_sata(child);
		if (res) {
			pr_notice("sas_discover_sata() for device %16llx at %016llx:%02d returned 0x%x\n",
				  SAS_ADDR(child->sas_addr),
				  SAS_ADDR(parent->sas_addr), phy_id, res);
			goto out_list_del;
		}
	} else
#endif
	  if (phy->attached_tproto & SAS_PROTOCOL_SSP) {
		child->dev_type = SAS_END_DEVICE;
		rphy = sas_end_device_alloc(phy->port);
		/* FIXME: error handling */
		if (unlikely(!rphy))
			goto out_free;
		child->tproto = phy->attached_tproto;
		sas_init_dev(child);

		child->rphy = rphy;
		get_device(&rphy->dev);
		rphy->identify.phy_identifier = phy_id;
		sas_fill_in_rphy(child, rphy);

		list_add_tail(&child->disco_list_node, &parent->port->disco_list);

		res = sas_discover_end_dev(child);
		if (res) {
			pr_notice("sas_discover_end_dev() for device %16llx at %016llx:%02d returned 0x%x\n",
				  SAS_ADDR(child->sas_addr),
				  SAS_ADDR(parent->sas_addr), phy_id, res);
			goto out_list_del;
		}
	} else {
		pr_notice("target proto 0x%x at %016llx:0x%x not handled\n",
			  phy->attached_tproto, SAS_ADDR(parent->sas_addr),
			  phy_id);
		goto out_free;
	}

	list_add_tail(&child->siblings, &parent_ex->children);
	return child;

 out_list_del:
	sas_rphy_free(child->rphy);
	list_del(&child->disco_list_node);
	spin_lock_irq(&parent->port->dev_list_lock);
	list_del(&child->dev_list_node);
	spin_unlock_irq(&parent->port->dev_list_lock);
 out_free:
	sas_port_delete(phy->port);
 out_err:
	phy->port = NULL;
	sas_put_device(child);
	return NULL;
}