static int sev_asid_new(struct kvm_sev_info *sev)
{
	int asid, min_asid, max_asid, ret;
	bool retry = true;
	enum misc_res_type type;

	type = sev->es_active ? MISC_CG_RES_SEV_ES : MISC_CG_RES_SEV;
	WARN_ON(sev->misc_cg);
	sev->misc_cg = get_current_misc_cg();
	ret = misc_cg_try_charge(type, sev->misc_cg, 1);
	if (ret) {
		put_misc_cg(sev->misc_cg);
		sev->misc_cg = NULL;
		return ret;
	}

	mutex_lock(&sev_bitmap_lock);

	/*
	 * SEV-enabled guests must use asid from min_sev_asid to max_sev_asid.
	 * SEV-ES-enabled guest can use from 1 to min_sev_asid - 1.
	 */
	min_asid = sev->es_active ? 1 : min_sev_asid;
	max_asid = sev->es_active ? min_sev_asid - 1 : max_sev_asid;
again:
	asid = find_next_zero_bit(sev_asid_bitmap, max_asid + 1, min_asid);
	if (asid > max_asid) {
		if (retry && __sev_recycle_asids(min_asid, max_asid)) {
			retry = false;
			goto again;
		}
		mutex_unlock(&sev_bitmap_lock);
		ret = -EBUSY;
		goto e_uncharge;
	}

	__set_bit(asid, sev_asid_bitmap);

	mutex_unlock(&sev_bitmap_lock);

	return asid;
e_uncharge:
	misc_cg_uncharge(type, sev->misc_cg, 1);
	put_misc_cg(sev->misc_cg);
	sev->misc_cg = NULL;
	return ret;
}