struct ath6kl_vif *ath6kl_get_vif_by_index(struct ath6kl *ar, u8 if_idx)
{
	struct ath6kl_vif *vif, *found = NULL;

	if (WARN_ON(if_idx > (ar->vif_max - 1)))
		return NULL;

	/* FIXME: Locking */
	spin_lock_bh(&ar->list_lock);
	list_for_each_entry(vif, &ar->vif_list, list) {
		if (vif->fw_vif_idx == if_idx) {
			found = vif;
			break;
		}
	}
	spin_unlock_bh(&ar->list_lock);

	return found;
}