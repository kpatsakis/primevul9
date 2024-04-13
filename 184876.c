bool jas_image_cmpt_domains_same(jas_image_t *image)
{
	int cmptno;
	jas_image_cmpt_t *cmpt;
	jas_image_cmpt_t *cmpt0;

	cmpt0 = image->cmpts_[0];
	for (cmptno = 1; cmptno < image->numcmpts_; ++cmptno) {
		cmpt = image->cmpts_[cmptno];
		if (cmpt->tlx_ != cmpt0->tlx_ || cmpt->tly_ != cmpt0->tly_ ||
		  cmpt->hstep_ != cmpt0->hstep_ || cmpt->vstep_ != cmpt0->vstep_ ||
		  cmpt->width_ != cmpt0->width_ || cmpt->height_ != cmpt0->height_) {
			return 0;
		}
	}
	return 1;
}