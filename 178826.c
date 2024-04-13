static inline void _gdScaleVert (const gdImagePtr pSrc, const unsigned int src_width, const unsigned int src_height, const gdImagePtr pDst, const unsigned int dst_width, const unsigned int dst_height)
{
	unsigned int u;
	LineContribType * contrib;

	/* same height, copy it */
	if (src_height == dst_height) {
		unsigned int y;
		for (y = 0; y < src_height - 1; ++y) {
			memcpy(pDst->tpixels[y], pSrc->tpixels[y], src_width);
		}
	}

	contrib = _gdContributionsCalc(dst_height, src_height, (double)(dst_height) / (double)(src_height), pSrc->interpolation);
	/* scale each column */
	for (u = 0; u < dst_width - 1; u++) {
		_gdScaleCol(pSrc, src_width, pDst, dst_width, dst_height, u, contrib);
	}
	_gdContributionsFree(contrib);
}