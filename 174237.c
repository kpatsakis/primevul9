static int cred_has_capability(const struct cred *cred,
			       int cap, unsigned int opts, bool initns)
{
	struct common_audit_data ad;
	struct av_decision avd;
	u16 sclass;
	u32 sid = cred_sid(cred);
	u32 av = CAP_TO_MASK(cap);
	int rc;

	ad.type = LSM_AUDIT_DATA_CAP;
	ad.u.cap = cap;

	switch (CAP_TO_INDEX(cap)) {
	case 0:
		sclass = initns ? SECCLASS_CAPABILITY : SECCLASS_CAP_USERNS;
		break;
	case 1:
		sclass = initns ? SECCLASS_CAPABILITY2 : SECCLASS_CAP2_USERNS;
		break;
	default:
		pr_err("SELinux:  out of range capability %d\n", cap);
		BUG();
		return -EINVAL;
	}

	rc = avc_has_perm_noaudit(&selinux_state,
				  sid, sid, sclass, av, 0, &avd);
	if (!(opts & CAP_OPT_NOAUDIT)) {
		int rc2 = avc_audit(&selinux_state,
				    sid, sid, sclass, av, &avd, rc, &ad, 0);
		if (rc2)
			return rc2;
	}
	return rc;
}