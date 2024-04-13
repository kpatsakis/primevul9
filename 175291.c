static int ovs_vport_cmd_new(struct sk_buff *skb, struct genl_info *info)
{
	struct nlattr **a = info->attrs;
	struct ovs_header *ovs_header = info->userhdr;
	struct vport_parms parms;
	struct sk_buff *reply;
	struct vport *vport;
	struct datapath *dp;
	u32 port_no;
	int err;

	err = -EINVAL;
	if (!a[OVS_VPORT_ATTR_NAME] || !a[OVS_VPORT_ATTR_TYPE] ||
	    !a[OVS_VPORT_ATTR_UPCALL_PID])
		goto exit;

	ovs_lock();
	dp = get_dp(sock_net(skb->sk), ovs_header->dp_ifindex);
	err = -ENODEV;
	if (!dp)
		goto exit_unlock;

	if (a[OVS_VPORT_ATTR_PORT_NO]) {
		port_no = nla_get_u32(a[OVS_VPORT_ATTR_PORT_NO]);

		err = -EFBIG;
		if (port_no >= DP_MAX_PORTS)
			goto exit_unlock;

		vport = ovs_vport_ovsl(dp, port_no);
		err = -EBUSY;
		if (vport)
			goto exit_unlock;
	} else {
		for (port_no = 1; ; port_no++) {
			if (port_no >= DP_MAX_PORTS) {
				err = -EFBIG;
				goto exit_unlock;
			}
			vport = ovs_vport_ovsl(dp, port_no);
			if (!vport)
				break;
		}
	}

	parms.name = nla_data(a[OVS_VPORT_ATTR_NAME]);
	parms.type = nla_get_u32(a[OVS_VPORT_ATTR_TYPE]);
	parms.options = a[OVS_VPORT_ATTR_OPTIONS];
	parms.dp = dp;
	parms.port_no = port_no;
	parms.upcall_portid = nla_get_u32(a[OVS_VPORT_ATTR_UPCALL_PID]);

	vport = new_vport(&parms);
	err = PTR_ERR(vport);
	if (IS_ERR(vport))
		goto exit_unlock;

	err = 0;
	reply = ovs_vport_cmd_build_info(vport, info->snd_portid, info->snd_seq,
					 OVS_VPORT_CMD_NEW);
	if (IS_ERR(reply)) {
		err = PTR_ERR(reply);
		ovs_dp_detach_port(vport);
		goto exit_unlock;
	}

	ovs_notify(&dp_vport_genl_family, reply, info);

exit_unlock:
	ovs_unlock();
exit:
	return err;
}