static int ovs_vport_cmd_fill_info(struct vport *vport, struct sk_buff *skb,
				   u32 portid, u32 seq, u32 flags, u8 cmd)
{
	struct ovs_header *ovs_header;
	struct ovs_vport_stats vport_stats;
	int err;

	ovs_header = genlmsg_put(skb, portid, seq, &dp_vport_genl_family,
				 flags, cmd);
	if (!ovs_header)
		return -EMSGSIZE;

	ovs_header->dp_ifindex = get_dpifindex(vport->dp);

	if (nla_put_u32(skb, OVS_VPORT_ATTR_PORT_NO, vport->port_no) ||
	    nla_put_u32(skb, OVS_VPORT_ATTR_TYPE, vport->ops->type) ||
	    nla_put_string(skb, OVS_VPORT_ATTR_NAME, vport->ops->get_name(vport)) ||
	    nla_put_u32(skb, OVS_VPORT_ATTR_UPCALL_PID, vport->upcall_portid))
		goto nla_put_failure;

	ovs_vport_get_stats(vport, &vport_stats);
	if (nla_put(skb, OVS_VPORT_ATTR_STATS, sizeof(struct ovs_vport_stats),
		    &vport_stats))
		goto nla_put_failure;

	err = ovs_vport_get_options(vport, skb);
	if (err == -EMSGSIZE)
		goto error;

	return genlmsg_end(skb, ovs_header);

nla_put_failure:
	err = -EMSGSIZE;
error:
	genlmsg_cancel(skb, ovs_header);
	return err;
}