static int bnx2x_ptp_enable(struct ptp_clock_info *ptp,
			    struct ptp_clock_request *rq, int on)
{
	struct bnx2x *bp = container_of(ptp, struct bnx2x, ptp_clock_info);

	BNX2X_ERR("PHC ancillary features are not supported\n");
	return -ENOTSUPP;
}