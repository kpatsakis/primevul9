static void i40e_delete_invalid_filter(struct i40e_pf *pf,
				       struct i40e_fdir_filter *filter)
{
	/* Update counters */
	pf->fdir_pf_active_filters--;
	pf->fd_inv = 0;

	switch (filter->flow_type) {
	case TCP_V4_FLOW:
		pf->fd_tcp4_filter_cnt--;
		break;
	case UDP_V4_FLOW:
		pf->fd_udp4_filter_cnt--;
		break;
	case SCTP_V4_FLOW:
		pf->fd_sctp4_filter_cnt--;
		break;
	case IP_USER_FLOW:
		switch (filter->ip4_proto) {
		case IPPROTO_TCP:
			pf->fd_tcp4_filter_cnt--;
			break;
		case IPPROTO_UDP:
			pf->fd_udp4_filter_cnt--;
			break;
		case IPPROTO_SCTP:
			pf->fd_sctp4_filter_cnt--;
			break;
		case IPPROTO_IP:
			pf->fd_ip4_filter_cnt--;
			break;
		}
		break;
	}

	/* Remove the filter from the list and free memory */
	hlist_del(&filter->fdir_node);
	kfree(filter);
}