void bns_dump(const bntseq_t *bns, const char *prefix)
{
	char str[1024];
	FILE *fp;
	int i;
	{ // dump .ann
		strcpy(str, prefix); strcat(str, ".ann");
		fp = xopen(str, "w");
		err_fprintf(fp, "%lld %d %u\n", (long long)bns->l_pac, bns->n_seqs, bns->seed);
		for (i = 0; i != bns->n_seqs; ++i) {
			bntann1_t *p = bns->anns + i;
			err_fprintf(fp, "%d %s", p->gi, p->name);
			if (p->anno[0]) err_fprintf(fp, " %s\n", p->anno);
			else err_fprintf(fp, "\n");
			err_fprintf(fp, "%lld %d %d\n", (long long)p->offset, p->len, p->n_ambs);
		}
		err_fflush(fp);
		err_fclose(fp);
	}
	{ // dump .amb
		strcpy(str, prefix); strcat(str, ".amb");
		fp = xopen(str, "w");
		err_fprintf(fp, "%lld %d %u\n", (long long)bns->l_pac, bns->n_seqs, bns->n_holes);
		for (i = 0; i != bns->n_holes; ++i) {
			bntamb1_t *p = bns->ambs + i;
			err_fprintf(fp, "%lld %d %c\n", (long long)p->offset, p->len, p->amb);
		}
		err_fflush(fp);
		err_fclose(fp);
	}
}