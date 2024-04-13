static int jpc_encrefpass(jpc_mqenc_t *mqenc, int bitpos, int vcausalflag, jas_matrix_t *flags, jas_matrix_t *data,
  int term, long *nmsedec)
{
	int i;
	int j;
	int one;
	int vscanlen;
	int d;
	int width;
	int height;
	int frowstep;
	int drowstep;
	int fstripestep;
	int dstripestep;
	jpc_fix_t *fstripestart;
	jpc_fix_t *dstripestart;
	jpc_fix_t *fvscanstart;
	jpc_fix_t *dvscanstart;
	jpc_fix_t *dp;
	jpc_fix_t *fp;
int k;

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
			dp = dvscanstart;
			k = vscanlen;

			refpass_step(fp, dp, bitpos, one, nmsedec,
			  mqenc, vcausalflag);
			if (--k <= 0) {
				continue;
			}
			fp += frowstep;
			dp += drowstep;
			refpass_step(fp, dp, bitpos, one, nmsedec,
			  mqenc, 0);
			if (--k <= 0) {
				continue;
			}
			fp += frowstep;
			dp += drowstep;
			refpass_step(fp, dp, bitpos, one, nmsedec,
			  mqenc, 0);
			if (--k <= 0) {
				continue;
			}
			fp += frowstep;
			dp += drowstep;
			refpass_step(fp, dp, bitpos, one, nmsedec,
			  mqenc, 0);

		}
	}

	if (term) {
		jpc_mqenc_flush(mqenc, term - 1);
	}

	return jpc_mqenc_error(mqenc) ? (-1) : 0;
}