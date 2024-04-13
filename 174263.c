static int selinux_key_permission(key_ref_t key_ref,
				  const struct cred *cred,
				  unsigned perm)
{
	struct key *key;
	struct key_security_struct *ksec;
	u32 sid;

	/* if no specific permissions are requested, we skip the
	   permission check. No serious, additional covert channels
	   appear to be created. */
	if (perm == 0)
		return 0;

	sid = cred_sid(cred);

	key = key_ref_to_ptr(key_ref);
	ksec = key->security;

	return avc_has_perm(&selinux_state,
			    sid, ksec->sid, SECCLASS_KEY, perm, NULL);
}