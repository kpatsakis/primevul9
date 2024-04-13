static int jpc_encclnpass(jpc_mqenc_t *mqenc, int bitpos, int orient, int vcausalflag, int segsymflag, jas_matrix_t *flags,
  jas_matrix_t *data, int term, long *nmsedec)
{
	int i;
	int j;
	int k;
	int vscanlen;
	int v;
	int runlen;
	jpc_fix_t *fp;
	int width;
	int height;
	jpc_fix_t *dp;
	int one;
	int frowstep;
	int drowstep;
	int fstripestep;
	int dstripestep;
	jpc_fix_t *fstripestart;
	jpc_fix_t *dstripestart;
	jpc_fix_t *fvscanstart;
	jpc_fix_t *dvscanstart;

	*nmsedec = 0;
	width = jas_matrix_numcols(data);
	height = jas_matrix_numrows(data);
	frowstep = jas_matrix_rowstep(flags);
	drowstep = jas_matrix_rowstep(data);
	fstripestep = frowstep << 2;
	dstripestep = drowstep << 2;

	one = 1 << (bitpos + JPC_NUMEXTRABITS);

	fstripestart = jas_matrix_getref(flags, 1, 1);
	dstripestart = jas_matrix_getref(data, 0, 0);
	for (i = height; i > 0; i -= 4, fstripestart += fstripestep,
	  dstripestart += dstripestep) {
		fvscanstart = fstripestart;
		dvscanstart = dstripestart;
		vscanlen = JAS_MIN(i, 4);
		for (j = width; j > 0; --j, ++fvscanstart, ++dvscanstart) {

			fp = fvscanstart;
			if (vscanlen >= 4 && !((*fp) & (JPC_SIG | JPC_VISIT |
			  JPC_OTHSIGMSK)) && (fp += frowstep, !((*fp) & (JPC_SIG |
			  JPC_VISIT | JPC_OTHSIGMSK))) && (fp += frowstep, !((*fp) &
			  (JPC_SIG | JPC_VISIT | JPC_OTHSIGMSK))) && (fp += frowstep,
			  !((*fp) & (JPC_SIG | JPC_VISIT | JPC_OTHSIGMSK)))) {
				dp = dvscanstart;
				for (k = 0; k < vscanlen; ++k) {
					v = (JAS_ABS(*dp) & one) ? 1 : 0;
					if (v) {
						break;
					}
					dp += drowstep;
				}
				runlen = k;
				if (runlen >= 4) {
					jpc_mqenc_setcurctx(mqenc, JPC_AGGCTXNO);
					jpc_mqenc_putbit(mqenc, 0);
					continue;
				}
				jpc_mqenc_setcurctx(mqenc, JPC_AGGCTXNO);
				jpc_mqenc_putbit(mqenc, 1);
				jpc_mqenc_setcurctx(mqenc, JPC_UCTXNO);
				jpc_mqenc_putbit(mqenc, runlen >> 1);
				jpc_mqenc_putbit(mqenc, runlen & 1);
				fp = fvscanstart + frowstep * runlen;
				dp = dvscanstart + drowstep * runlen;
				k = vscanlen - runlen;
				switch (runlen) {
				case 0:
					goto clnpass_partial0;
					break;
				case 1:
					goto clnpass_partial1;
					break;
				case 2:
					goto clnpass_partial2;
					break;
				case 3:
					goto clnpass_partial3;
					break;
				}
			} else {
				runlen = 0;
				fp = fvscanstart;
				dp = dvscanstart;
				k = vscanlen;
				goto clnpass_full0;
			}
			clnpass_step(fp, frowstep, dp, bitpos, one,
			  orient, nmsedec, mqenc, clnpass_full0:, clnpass_partial0:, vcausalflag);
			if (--k <= 0) {
				continue;
			}
			fp += frowstep;
			dp += drowstep;
			clnpass_step(fp, frowstep, dp, bitpos, one,
				orient, nmsedec, mqenc, ;, clnpass_partial1:, 0);
			if (--k <= 0) {
				continue;
			}
			fp += frowstep;
			dp += drowstep;
			clnpass_step(fp, frowstep, dp, bitpos, one,
				orient, nmsedec, mqenc, ;, clnpass_partial2:, 0);
			if (--k <= 0) {
				continue;
			}
			fp += frowstep;
			dp += drowstep;
			clnpass_step(fp, frowstep, dp, bitpos, one,
				orient, nmsedec, mqenc, ;, clnpass_partial3:, 0);
		}
	}

	if (segsymflag) {
		jpc_mqenc_setcurctx(mqenc, JPC_UCTXNO);
		jpc_mqenc_putbit(mqenc, 1);
		jpc_mqenc_putbit(mqenc, 0);
		jpc_mqenc_putbit(mqenc, 1);
		jpc_mqenc_putbit(mqenc, 0);
	}

	if (term) {
		jpc_mqenc_flush(mqenc, term - 1);
	}

	return jpc_mqenc_error(mqenc) ? (-1) : 0;
}