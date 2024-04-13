static int compute_bitstructs(struct gfs2_rgrpd *rgd)
{
	struct gfs2_sbd *sdp = rgd->rd_sbd;
	struct gfs2_bitmap *bi;
	u32 length = rgd->rd_length; /* # blocks in hdr & bitmap */
	u32 bytes_left, bytes;
	int x;

	if (!length)
		return -EINVAL;

	rgd->rd_bits = kcalloc(length, sizeof(struct gfs2_bitmap), GFP_NOFS);
	if (!rgd->rd_bits)
		return -ENOMEM;

	bytes_left = rgd->rd_bitbytes;

	for (x = 0; x < length; x++) {
		bi = rgd->rd_bits + x;

		bi->bi_flags = 0;
		/* small rgrp; bitmap stored completely in header block */
		if (length == 1) {
			bytes = bytes_left;
			bi->bi_offset = sizeof(struct gfs2_rgrp);
			bi->bi_start = 0;
			bi->bi_len = bytes;
			bi->bi_blocks = bytes * GFS2_NBBY;
		/* header block */
		} else if (x == 0) {
			bytes = sdp->sd_sb.sb_bsize - sizeof(struct gfs2_rgrp);
			bi->bi_offset = sizeof(struct gfs2_rgrp);
			bi->bi_start = 0;
			bi->bi_len = bytes;
			bi->bi_blocks = bytes * GFS2_NBBY;
		/* last block */
		} else if (x + 1 == length) {
			bytes = bytes_left;
			bi->bi_offset = sizeof(struct gfs2_meta_header);
			bi->bi_start = rgd->rd_bitbytes - bytes_left;
			bi->bi_len = bytes;
			bi->bi_blocks = bytes * GFS2_NBBY;
		/* other blocks */
		} else {
			bytes = sdp->sd_sb.sb_bsize -
				sizeof(struct gfs2_meta_header);
			bi->bi_offset = sizeof(struct gfs2_meta_header);
			bi->bi_start = rgd->rd_bitbytes - bytes_left;
			bi->bi_len = bytes;
			bi->bi_blocks = bytes * GFS2_NBBY;
		}

		bytes_left -= bytes;
	}

	if (bytes_left) {
		gfs2_consist_rgrpd(rgd);
		return -EIO;
	}
	bi = rgd->rd_bits + (length - 1);
	if ((bi->bi_start + bi->bi_len) * GFS2_NBBY != rgd->rd_data) {
		if (gfs2_consist_rgrpd(rgd)) {
			gfs2_rindex_print(rgd);
			fs_err(sdp, "start=%u len=%u offset=%u\n",
			       bi->bi_start, bi->bi_len, bi->bi_offset);
		}
		return -EIO;
	}

	return 0;
}