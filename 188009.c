static void prb_init_ft_ops(struct kbdq_core *p1,
			union tpacket_req_u *req_u)
{
	p1->feature_req_word = req_u->req3.tp_feature_req_word;
}