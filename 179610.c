int64_t bns_fasta2bntseq(gzFile fp_fa, const char *prefix, int for_only)
{
	extern void seq_reverse(int len, ubyte_t *seq, int is_comp); // in bwaseqio.c
	kseq_t *seq;
	char name[1024];
	bntseq_t *bns;
	uint8_t *pac = 0;
	int32_t m_seqs, m_holes;
	int64_t ret = -1, m_pac, l;
	bntamb1_t *q;
	FILE *fp;

	// initialization
	seq = kseq_init(fp_fa);
	bns = (bntseq_t*)calloc(1, sizeof(bntseq_t));
	bns->seed = 11; // fixed seed for random generator
	srand48(bns->seed);
	m_seqs = m_holes = 8; m_pac = 0x10000;
	bns->anns = (bntann1_t*)calloc(m_seqs, sizeof(bntann1_t));
	bns->ambs = (bntamb1_t*)calloc(m_holes, sizeof(bntamb1_t));
	pac = calloc(m_pac/4, 1);
	q = bns->ambs;
	strcpy(name, prefix); strcat(name, ".pac");
	fp = xopen(name, "wb");
	// read sequences
	while (kseq_read(seq) >= 0) pac = add1(seq, bns, pac, &m_pac, &m_seqs, &m_holes, &q);
	if (!for_only) { // add the reverse complemented sequence
		int64_t ll_pac = (bns->l_pac * 2 + 3) / 4 * 4;
		if (ll_pac > m_pac) pac = realloc(pac, ll_pac/4);
		memset(pac + (bns->l_pac+3)/4, 0, (ll_pac - (bns->l_pac+3)/4*4) / 4);
		for (l = bns->l_pac - 1; l >= 0; --l, ++bns->l_pac)
			_set_pac(pac, bns->l_pac, 3-_get_pac(pac, l));
	}
	ret = bns->l_pac;
	{ // finalize .pac file
		ubyte_t ct;
		err_fwrite(pac, 1, (bns->l_pac>>2) + ((bns->l_pac&3) == 0? 0 : 1), fp);
		// the following codes make the pac file size always (l_pac/4+1+1)
		if (bns->l_pac % 4 == 0) {
			ct = 0;
			err_fwrite(&ct, 1, 1, fp);
		}
		ct = bns->l_pac % 4;
		err_fwrite(&ct, 1, 1, fp);
		// close .pac file
		err_fflush(fp);
		err_fclose(fp);
	}
	bns_dump(bns, prefix);
	bns_destroy(bns);
	kseq_destroy(seq);
	free(pac);
	return ret;
}