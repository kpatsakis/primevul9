static int rds_cong_monitor(struct rds_sock *rs, char __user *optval,
			    int optlen)
{
	int ret;

	ret = rds_set_bool_option(&rs->rs_cong_monitor, optval, optlen);
	if (ret == 0) {
		if (rs->rs_cong_monitor) {
			rds_cong_add_socket(rs);
		} else {
			rds_cong_remove_socket(rs);
			rs->rs_cong_mask = 0;
			rs->rs_cong_notify = 0;
		}
	}
	return ret;
}