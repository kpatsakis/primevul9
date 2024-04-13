static int selinux_ib_endport_manage_subnet(void *ib_sec, const char *dev_name,
					    u8 port_num)
{
	struct common_audit_data ad;
	int err;
	u32 sid = 0;
	struct ib_security_struct *sec = ib_sec;
	struct lsm_ibendport_audit ibendport;

	err = security_ib_endport_sid(&selinux_state, dev_name, port_num,
				      &sid);

	if (err)
		return err;

	ad.type = LSM_AUDIT_DATA_IBENDPORT;
	strncpy(ibendport.dev_name, dev_name, sizeof(ibendport.dev_name));
	ibendport.port = port_num;
	ad.u.ibendport = &ibendport;
	return avc_has_perm(&selinux_state,
			    sec->sid, sid,
			    SECCLASS_INFINIBAND_ENDPORT,
			    INFINIBAND_ENDPORT__MANAGE_SUBNET, &ad);
}