static struct rt6_info *find_match(struct rt6_info *rt, int oif, int strict,
				   int *mpri, struct rt6_info *match,
				   bool *do_rr)
{
	int m;
	bool match_do_rr = false;

	if (rt6_check_expired(rt))
		goto out;

	m = rt6_score_route(rt, oif, strict);
	if (m == RT6_NUD_FAIL_DO_RR) {
		match_do_rr = true;
		m = 0; /* lowest valid score */
	} else if (m == RT6_NUD_FAIL_HARD) {
		goto out;
	}

	if (strict & RT6_LOOKUP_F_REACHABLE)
		rt6_probe(rt);

	/* note that m can be RT6_NUD_FAIL_PROBE at this point */
	if (m > *mpri) {
		*do_rr = match_do_rr;
		*mpri = m;
		match = rt;
	}
out:
	return match;
}