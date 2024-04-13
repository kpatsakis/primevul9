int jpc_pi_next(jpc_pi_t *pi)
{
	jpc_pchg_t *pchg;
	int ret;


	for (;;) {

		pi->valid = false;

		if (!pi->pchg) {
			++pi->pchgno;
			pi->compno = 0;
			pi->rlvlno = 0;
			pi->prcno = 0;
			pi->lyrno = 0;
			pi->prgvolfirst = true;
			if (pi->pchgno < jpc_pchglist_numpchgs(pi->pchglist)) {
				pi->pchg = jpc_pchglist_get(pi->pchglist, pi->pchgno);
			} else if (pi->pchgno == jpc_pchglist_numpchgs(pi->pchglist)) {
				pi->pchg = &pi->defaultpchg;
			} else {
				return 1;
			}
		}

		pchg = pi->pchg;
		switch (pchg->prgord) {
		case JPC_COD_LRCPPRG:
			ret = jpc_pi_nextlrcp(pi);
			break;
		case JPC_COD_RLCPPRG:
			ret = jpc_pi_nextrlcp(pi);
			break;
		case JPC_COD_RPCLPRG:
			ret = jpc_pi_nextrpcl(pi);
			break;
		case JPC_COD_PCRLPRG:
			ret = jpc_pi_nextpcrl(pi);
			break;
		case JPC_COD_CPRLPRG:
			ret = jpc_pi_nextcprl(pi);
			break;
		default:
			ret = -1;
			break;
		}
		if (!ret) {
			pi->valid = true;
			++pi->pktno;
			return 0;
		}
		pi->pchg = 0;
	}
}