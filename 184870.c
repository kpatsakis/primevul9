void jas_image_dump(jas_image_t *image, FILE *out)
{
	long buf[1024];
	int cmptno;
	int n;
	int i;
	int width;
	int height;
	jas_image_cmpt_t *cmpt;
	for (cmptno = 0; cmptno < image->numcmpts_; ++cmptno) {
		cmpt = image->cmpts_[cmptno];
		fprintf(out, "prec=%d, sgnd=%d, cmpttype=%"PRIiFAST32"\n", cmpt->prec_,
		  cmpt->sgnd_, cmpt->type_);
		width = jas_image_cmptwidth(image, cmptno);
		height = jas_image_cmptheight(image, cmptno);
		n = JAS_MIN(16, width);
		if (jas_image_readcmpt2(image, cmptno, 0, 0, n, 1, buf)) {
			abort();
		}
		for (i = 0; i < n; ++i) {
			fprintf(out, " f(%d,%d)=%ld", i, 0, buf[i]);
		}
		fprintf(out, "\n");
		if (jas_image_readcmpt2(image, cmptno, width - n, height - 1, n, 1, buf)) {
			abort();
		}
		for (i = 0; i < n; ++i) {
			fprintf(out, " f(%d,%d)=%ld", width - n + i, height - 1, buf[i]);
		}
		fprintf(out, "\n");
	}
}