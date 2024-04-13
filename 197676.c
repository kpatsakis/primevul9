nfsd4_proc_compound(struct svc_rqst *rqstp,
		    struct nfsd4_compoundargs *args,
		    struct nfsd4_compoundres *resp)
{
	struct nfsd4_op	*op;
	struct nfsd4_operation *opdesc;
	struct nfsd4_compound_state *cstate = &resp->cstate;
	struct svc_fh *current_fh = &cstate->current_fh;
	struct svc_fh *save_fh = &cstate->save_fh;
	__be32		status;

	svcxdr_init_encode(rqstp, resp);
	resp->tagp = resp->xdr.p;
	/* reserve space for: taglen, tag, and opcnt */
	xdr_reserve_space(&resp->xdr, 8 + args->taglen);
	resp->taglen = args->taglen;
	resp->tag = args->tag;
	resp->rqstp = rqstp;
	cstate->minorversion = args->minorversion;
	fh_init(current_fh, NFS4_FHSIZE);
	fh_init(save_fh, NFS4_FHSIZE);
	/*
	 * Don't use the deferral mechanism for NFSv4; compounds make it
	 * too hard to avoid non-idempotency problems.
	 */
	clear_bit(RQ_USEDEFERRAL, &rqstp->rq_flags);

	/*
	 * According to RFC3010, this takes precedence over all other errors.
	 */
	status = nfserr_minor_vers_mismatch;
	if (nfsd_minorversion(args->minorversion, NFSD_TEST) <= 0)
		goto out;

	status = nfs41_check_op_ordering(args);
	if (status) {
		op = &args->ops[0];
		op->status = status;
		goto encode_op;
	}

	while (!status && resp->opcnt < args->opcnt) {
		op = &args->ops[resp->opcnt++];

		dprintk("nfsv4 compound op #%d/%d: %d (%s)\n",
			resp->opcnt, args->opcnt, op->opnum,
			nfsd4_op_name(op->opnum));
		/*
		 * The XDR decode routines may have pre-set op->status;
		 * for example, if there is a miscellaneous XDR error
		 * it will be set to nfserr_bad_xdr.
		 */
		if (op->status) {
			if (op->opnum == OP_OPEN)
				op->status = nfsd4_open_omfg(rqstp, cstate, op);
			goto encode_op;
		}

		opdesc = OPDESC(op);

		if (!current_fh->fh_dentry) {
			if (!(opdesc->op_flags & ALLOWED_WITHOUT_FH)) {
				op->status = nfserr_nofilehandle;
				goto encode_op;
			}
		} else if (current_fh->fh_export->ex_fslocs.migrated &&
			  !(opdesc->op_flags & ALLOWED_ON_ABSENT_FS)) {
			op->status = nfserr_moved;
			goto encode_op;
		}

		fh_clear_wcc(current_fh);

		/* If op is non-idempotent */
		if (opdesc->op_flags & OP_MODIFIES_SOMETHING) {
			/*
			 * Don't execute this op if we couldn't encode a
			 * succesful reply:
			 */
			u32 plen = opdesc->op_rsize_bop(rqstp, op);
			/*
			 * Plus if there's another operation, make sure
			 * we'll have space to at least encode an error:
			 */
			if (resp->opcnt < args->opcnt)
				plen += COMPOUND_ERR_SLACK_SPACE;
			op->status = nfsd4_check_resp_size(resp, plen);
		}

		if (op->status)
			goto encode_op;

		if (opdesc->op_get_currentstateid)
			opdesc->op_get_currentstateid(cstate, &op->u);
		op->status = opdesc->op_func(rqstp, cstate, &op->u);

		if (!op->status) {
			if (opdesc->op_set_currentstateid)
				opdesc->op_set_currentstateid(cstate, &op->u);

			if (opdesc->op_flags & OP_CLEAR_STATEID)
				clear_current_stateid(cstate);

			if (need_wrongsec_check(rqstp))
				op->status = check_nfsd_access(current_fh->fh_export, rqstp);
		}

encode_op:
		/* Only from SEQUENCE */
		if (cstate->status == nfserr_replay_cache) {
			dprintk("%s NFS4.1 replay from cache\n", __func__);
			status = op->status;
			goto out;
		}
		if (op->status == nfserr_replay_me) {
			op->replay = &cstate->replay_owner->so_replay;
			nfsd4_encode_replay(&resp->xdr, op);
			status = op->status = op->replay->rp_status;
		} else {
			nfsd4_encode_operation(resp, op);
			status = op->status;
		}

		dprintk("nfsv4 compound op %p opcnt %d #%d: %d: status %d\n",
			args->ops, args->opcnt, resp->opcnt, op->opnum,
			be32_to_cpu(status));

		nfsd4_cstate_clear_replay(cstate);
		nfsd4_increment_op_stats(op->opnum);
	}

	cstate->status = status;
	fh_put(current_fh);
	fh_put(save_fh);
	BUG_ON(cstate->replay_owner);
out:
	/* Reset deferral mechanism for RPC deferrals */
	set_bit(RQ_USEDEFERRAL, &rqstp->rq_flags);
	dprintk("nfsv4 compound returned %d\n", ntohl(status));
	return status;
}