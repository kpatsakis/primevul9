static uint8_t *add1(const kseq_t *seq, bntseq_t *bns, uint8_t *pac, int64_t *m_pac, int *m_seqs, int *m_holes, bntamb1_t **q)
{
	bntann1_t *p;
	int i, lasts;
	if (bns->n_seqs == *m_seqs) {
		*m_seqs <<= 1;
		bns->anns = (bntann1_t*)realloc(bns->anns, *m_seqs * sizeof(bntann1_t));
	}
	p = bns->anns + bns->n_seqs;
	p->name = strdup((char*)seq->name.s);
	p->anno = seq->comment.l > 0? strdup((char*)seq->comment.s) : strdup("(null)");
	p->gi = 0; p->len = seq->seq.l;
	p->offset = (bns->n_seqs == 0)? 0 : (p-1)->offset + (p-1)->len;
	p->n_ambs = 0;
	for (i = lasts = 0; i < seq->seq.l; ++i) {
		int c = nst_nt4_table[(int)seq->seq.s[i]];
		if (c >= 4) { // N
			if (lasts == seq->seq.s[i]) { // contiguous N
				++(*q)->len;
			} else {
				if (bns->n_holes == *m_holes) {
					(*m_holes) <<= 1;
					bns->ambs = (bntamb1_t*)realloc(bns->ambs, (*m_holes) * sizeof(bntamb1_t));
				}
				*q = bns->ambs + bns->n_holes;
				(*q)->len = 1;
				(*q)->offset = p->offset + i;
				(*q)->amb = seq->seq.s[i];
				++p->n_ambs;
				++bns->n_holes;
			}
		}
		lasts = seq->seq.s[i];
		{ // fill buffer
			if (c >= 4) c = lrand48()&3;
			if (bns->l_pac == *m_pac) { // double the pac size
				*m_pac <<= 1;
				pac = realloc(pac, *m_pac/4);
				memset(pac + bns->l_pac/4, 0, (*m_pac - bns->l_pac)/4);
			}
			_set_pac(pac, bns->l_pac, c);
			++bns->l_pac;
		}
	}
	++bns->n_seqs;
	return pac;
}