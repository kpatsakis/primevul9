nfp_abm_u32_check_knode(struct nfp_abm *abm, struct tc_cls_u32_knode *knode,
			__be16 proto, struct netlink_ext_ack *extack)
{
	struct tc_u32_key *k;
	unsigned int tos_off;

	if (knode->exts && tcf_exts_has_actions(knode->exts)) {
		NL_SET_ERR_MSG_MOD(extack, "action offload not supported");
		return false;
	}
	if (knode->link_handle) {
		NL_SET_ERR_MSG_MOD(extack, "linking not supported");
		return false;
	}
	if (knode->sel->flags != TC_U32_TERMINAL) {
		NL_SET_ERR_MSG_MOD(extack,
				   "flags must be equal to TC_U32_TERMINAL");
		return false;
	}
	if (knode->sel->off || knode->sel->offshift || knode->sel->offmask ||
	    knode->sel->offoff || knode->fshift) {
		NL_SET_ERR_MSG_MOD(extack, "variable offsetting not supported");
		return false;
	}
	if (knode->sel->hoff || knode->sel->hmask) {
		NL_SET_ERR_MSG_MOD(extack, "hashing not supported");
		return false;
	}
	if (knode->val || knode->mask) {
		NL_SET_ERR_MSG_MOD(extack, "matching on mark not supported");
		return false;
	}
	if (knode->res && knode->res->class) {
		NL_SET_ERR_MSG_MOD(extack, "setting non-0 class not supported");
		return false;
	}
	if (knode->res && knode->res->classid >= abm->num_bands) {
		NL_SET_ERR_MSG_MOD(extack,
				   "classid higher than number of bands");
		return false;
	}
	if (knode->sel->nkeys != 1) {
		NL_SET_ERR_MSG_MOD(extack, "exactly one key required");
		return false;
	}

	switch (proto) {
	case htons(ETH_P_IP):
		tos_off = 16;
		break;
	case htons(ETH_P_IPV6):
		tos_off = 20;
		break;
	default:
		NL_SET_ERR_MSG_MOD(extack, "only IP and IPv6 supported as filter protocol");
		return false;
	}

	k = &knode->sel->keys[0];
	if (k->offmask) {
		NL_SET_ERR_MSG_MOD(extack, "offset mask - variable offsetting not supported");
		return false;
	}
	if (k->off) {
		NL_SET_ERR_MSG_MOD(extack, "only DSCP fields can be matched");
		return false;
	}
	if (k->val & ~k->mask) {
		NL_SET_ERR_MSG_MOD(extack, "mask does not cover the key");
		return false;
	}
	if (be32_to_cpu(k->mask) >> tos_off & ~abm->dscp_mask) {
		NL_SET_ERR_MSG_MOD(extack, "only high DSCP class selector bits can be used");
		nfp_err(abm->app->cpp,
			"u32 offload: requested mask %x FW can support only %x\n",
			be32_to_cpu(k->mask) >> tos_off, abm->dscp_mask);
		return false;
	}

	return true;
}