int i40e_sync_vsi_filters(struct i40e_vsi *vsi)
{
	struct hlist_head tmp_add_list, tmp_del_list;
	struct i40e_mac_filter *f;
	struct i40e_new_mac_filter *new, *add_head = NULL;
	struct i40e_hw *hw = &vsi->back->hw;
	bool old_overflow, new_overflow;
	unsigned int failed_filters = 0;
	unsigned int vlan_filters = 0;
	char vsi_name[16] = "PF";
	int filter_list_len = 0;
	i40e_status aq_ret = 0;
	u32 changed_flags = 0;
	struct hlist_node *h;
	struct i40e_pf *pf;
	int num_add = 0;
	int num_del = 0;
	int retval = 0;
	u16 cmd_flags;
	int list_size;
	int bkt;

	/* empty array typed pointers, kcalloc later */
	struct i40e_aqc_add_macvlan_element_data *add_list;
	struct i40e_aqc_remove_macvlan_element_data *del_list;

	while (test_and_set_bit(__I40E_VSI_SYNCING_FILTERS, vsi->state))
		usleep_range(1000, 2000);
	pf = vsi->back;

	old_overflow = test_bit(__I40E_VSI_OVERFLOW_PROMISC, vsi->state);

	if (vsi->netdev) {
		changed_flags = vsi->current_netdev_flags ^ vsi->netdev->flags;
		vsi->current_netdev_flags = vsi->netdev->flags;
	}

	INIT_HLIST_HEAD(&tmp_add_list);
	INIT_HLIST_HEAD(&tmp_del_list);

	if (vsi->type == I40E_VSI_SRIOV)
		snprintf(vsi_name, sizeof(vsi_name) - 1, "VF %d", vsi->vf_id);
	else if (vsi->type != I40E_VSI_MAIN)
		snprintf(vsi_name, sizeof(vsi_name) - 1, "vsi %d", vsi->seid);

	if (vsi->flags & I40E_VSI_FLAG_FILTER_CHANGED) {
		vsi->flags &= ~I40E_VSI_FLAG_FILTER_CHANGED;

		spin_lock_bh(&vsi->mac_filter_hash_lock);
		/* Create a list of filters to delete. */
		hash_for_each_safe(vsi->mac_filter_hash, bkt, h, f, hlist) {
			if (f->state == I40E_FILTER_REMOVE) {
				/* Move the element into temporary del_list */
				hash_del(&f->hlist);
				hlist_add_head(&f->hlist, &tmp_del_list);

				/* Avoid counting removed filters */
				continue;
			}
			if (f->state == I40E_FILTER_NEW) {
				/* Create a temporary i40e_new_mac_filter */
				new = kzalloc(sizeof(*new), GFP_ATOMIC);
				if (!new)
					goto err_no_memory_locked;

				/* Store pointer to the real filter */
				new->f = f;
				new->state = f->state;

				/* Add it to the hash list */
				hlist_add_head(&new->hlist, &tmp_add_list);
			}

			/* Count the number of active (current and new) VLAN
			 * filters we have now. Does not count filters which
			 * are marked for deletion.
			 */
			if (f->vlan > 0)
				vlan_filters++;
		}

		retval = i40e_correct_mac_vlan_filters(vsi,
						       &tmp_add_list,
						       &tmp_del_list,
						       vlan_filters);
		if (retval)
			goto err_no_memory_locked;

		spin_unlock_bh(&vsi->mac_filter_hash_lock);
	}

	/* Now process 'del_list' outside the lock */
	if (!hlist_empty(&tmp_del_list)) {
		filter_list_len = hw->aq.asq_buf_size /
			    sizeof(struct i40e_aqc_remove_macvlan_element_data);
		list_size = filter_list_len *
			    sizeof(struct i40e_aqc_remove_macvlan_element_data);
		del_list = kzalloc(list_size, GFP_ATOMIC);
		if (!del_list)
			goto err_no_memory;

		hlist_for_each_entry_safe(f, h, &tmp_del_list, hlist) {
			cmd_flags = 0;

			/* handle broadcast filters by updating the broadcast
			 * promiscuous flag and release filter list.
			 */
			if (is_broadcast_ether_addr(f->macaddr)) {
				i40e_aqc_broadcast_filter(vsi, vsi_name, f);

				hlist_del(&f->hlist);
				kfree(f);
				continue;
			}

			/* add to delete list */
			ether_addr_copy(del_list[num_del].mac_addr, f->macaddr);
			if (f->vlan == I40E_VLAN_ANY) {
				del_list[num_del].vlan_tag = 0;
				cmd_flags |= I40E_AQC_MACVLAN_DEL_IGNORE_VLAN;
			} else {
				del_list[num_del].vlan_tag =
					cpu_to_le16((u16)(f->vlan));
			}

			cmd_flags |= I40E_AQC_MACVLAN_DEL_PERFECT_MATCH;
			del_list[num_del].flags = cmd_flags;
			num_del++;

			/* flush a full buffer */
			if (num_del == filter_list_len) {
				i40e_aqc_del_filters(vsi, vsi_name, del_list,
						     num_del, &retval);
				memset(del_list, 0, list_size);
				num_del = 0;
			}
			/* Release memory for MAC filter entries which were
			 * synced up with HW.
			 */
			hlist_del(&f->hlist);
			kfree(f);
		}

		if (num_del) {
			i40e_aqc_del_filters(vsi, vsi_name, del_list,
					     num_del, &retval);
		}

		kfree(del_list);
		del_list = NULL;
	}

	if (!hlist_empty(&tmp_add_list)) {
		/* Do all the adds now. */
		filter_list_len = hw->aq.asq_buf_size /
			       sizeof(struct i40e_aqc_add_macvlan_element_data);
		list_size = filter_list_len *
			       sizeof(struct i40e_aqc_add_macvlan_element_data);
		add_list = kzalloc(list_size, GFP_ATOMIC);
		if (!add_list)
			goto err_no_memory;

		num_add = 0;
		hlist_for_each_entry_safe(new, h, &tmp_add_list, hlist) {
			/* handle broadcast filters by updating the broadcast
			 * promiscuous flag instead of adding a MAC filter.
			 */
			if (is_broadcast_ether_addr(new->f->macaddr)) {
				if (i40e_aqc_broadcast_filter(vsi, vsi_name,
							      new->f))
					new->state = I40E_FILTER_FAILED;
				else
					new->state = I40E_FILTER_ACTIVE;
				continue;
			}

			/* add to add array */
			if (num_add == 0)
				add_head = new;
			cmd_flags = 0;
			ether_addr_copy(add_list[num_add].mac_addr,
					new->f->macaddr);
			if (new->f->vlan == I40E_VLAN_ANY) {
				add_list[num_add].vlan_tag = 0;
				cmd_flags |= I40E_AQC_MACVLAN_ADD_IGNORE_VLAN;
			} else {
				add_list[num_add].vlan_tag =
					cpu_to_le16((u16)(new->f->vlan));
			}
			add_list[num_add].queue_number = 0;
			/* set invalid match method for later detection */
			add_list[num_add].match_method = I40E_AQC_MM_ERR_NO_RES;
			cmd_flags |= I40E_AQC_MACVLAN_ADD_PERFECT_MATCH;
			add_list[num_add].flags = cpu_to_le16(cmd_flags);
			num_add++;

			/* flush a full buffer */
			if (num_add == filter_list_len) {
				i40e_aqc_add_filters(vsi, vsi_name, add_list,
						     add_head, num_add);
				memset(add_list, 0, list_size);
				num_add = 0;
			}
		}
		if (num_add) {
			i40e_aqc_add_filters(vsi, vsi_name, add_list, add_head,
					     num_add);
		}
		/* Now move all of the filters from the temp add list back to
		 * the VSI's list.
		 */
		spin_lock_bh(&vsi->mac_filter_hash_lock);
		hlist_for_each_entry_safe(new, h, &tmp_add_list, hlist) {
			/* Only update the state if we're still NEW */
			if (new->f->state == I40E_FILTER_NEW)
				new->f->state = new->state;
			hlist_del(&new->hlist);
			kfree(new);
		}
		spin_unlock_bh(&vsi->mac_filter_hash_lock);
		kfree(add_list);
		add_list = NULL;
	}

	/* Determine the number of active and failed filters. */
	spin_lock_bh(&vsi->mac_filter_hash_lock);
	vsi->active_filters = 0;
	hash_for_each(vsi->mac_filter_hash, bkt, f, hlist) {
		if (f->state == I40E_FILTER_ACTIVE)
			vsi->active_filters++;
		else if (f->state == I40E_FILTER_FAILED)
			failed_filters++;
	}
	spin_unlock_bh(&vsi->mac_filter_hash_lock);

	/* Check if we are able to exit overflow promiscuous mode. We can
	 * safely exit if we didn't just enter, we no longer have any failed
	 * filters, and we have reduced filters below the threshold value.
	 */
	if (old_overflow && !failed_filters &&
	    vsi->active_filters < vsi->promisc_threshold) {
		dev_info(&pf->pdev->dev,
			 "filter logjam cleared on %s, leaving overflow promiscuous mode\n",
			 vsi_name);
		clear_bit(__I40E_VSI_OVERFLOW_PROMISC, vsi->state);
		vsi->promisc_threshold = 0;
	}

	/* if the VF is not trusted do not do promisc */
	if ((vsi->type == I40E_VSI_SRIOV) && !pf->vf[vsi->vf_id].trusted) {
		clear_bit(__I40E_VSI_OVERFLOW_PROMISC, vsi->state);
		goto out;
	}

	new_overflow = test_bit(__I40E_VSI_OVERFLOW_PROMISC, vsi->state);

	/* If we are entering overflow promiscuous, we need to calculate a new
	 * threshold for when we are safe to exit
	 */
	if (!old_overflow && new_overflow)
		vsi->promisc_threshold = (vsi->active_filters * 3) / 4;

	/* check for changes in promiscuous modes */
	if (changed_flags & IFF_ALLMULTI) {
		bool cur_multipromisc;

		cur_multipromisc = !!(vsi->current_netdev_flags & IFF_ALLMULTI);
		aq_ret = i40e_aq_set_vsi_multicast_promiscuous(&vsi->back->hw,
							       vsi->seid,
							       cur_multipromisc,
							       NULL);
		if (aq_ret) {
			retval = i40e_aq_rc_to_posix(aq_ret,
						     hw->aq.asq_last_status);
			dev_info(&pf->pdev->dev,
				 "set multi promisc failed on %s, err %s aq_err %s\n",
				 vsi_name,
				 i40e_stat_str(hw, aq_ret),
				 i40e_aq_str(hw, hw->aq.asq_last_status));
		} else {
			dev_info(&pf->pdev->dev, "%s is %s allmulti mode.\n",
				 vsi->netdev->name,
				 cur_multipromisc ? "entering" : "leaving");
		}
	}

	if ((changed_flags & IFF_PROMISC) || old_overflow != new_overflow) {
		bool cur_promisc;

		cur_promisc = (!!(vsi->current_netdev_flags & IFF_PROMISC) ||
			       new_overflow);
		aq_ret = i40e_set_promiscuous(pf, cur_promisc);
		if (aq_ret) {
			retval = i40e_aq_rc_to_posix(aq_ret,
						     hw->aq.asq_last_status);
			dev_info(&pf->pdev->dev,
				 "Setting promiscuous %s failed on %s, err %s aq_err %s\n",
				 cur_promisc ? "on" : "off",
				 vsi_name,
				 i40e_stat_str(hw, aq_ret),
				 i40e_aq_str(hw, hw->aq.asq_last_status));
		}
	}
out:
	/* if something went wrong then set the changed flag so we try again */
	if (retval)
		vsi->flags |= I40E_VSI_FLAG_FILTER_CHANGED;

	clear_bit(__I40E_VSI_SYNCING_FILTERS, vsi->state);
	return retval;

err_no_memory:
	/* Restore elements on the temporary add and delete lists */
	spin_lock_bh(&vsi->mac_filter_hash_lock);
err_no_memory_locked:
	i40e_undo_del_filter_entries(vsi, &tmp_del_list);
	i40e_undo_add_filter_entries(vsi, &tmp_add_list);
	spin_unlock_bh(&vsi->mac_filter_hash_lock);

	vsi->flags |= I40E_VSI_FLAG_FILTER_CHANGED;
	clear_bit(__I40E_VSI_SYNCING_FILTERS, vsi->state);
	return -ENOMEM;
}