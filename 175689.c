static int ras_puthdr(jas_stream_t *out, ras_hdr_t *hdr)
{
	if (ras_putint(out, RAS_MAGIC) || ras_putint(out, hdr->width) ||
	  ras_putint(out, hdr->height) || ras_putint(out, hdr->depth) ||
	  ras_putint(out, hdr->length) || ras_putint(out, hdr->type) ||
	  ras_putint(out, hdr->maptype) || ras_putint(out, hdr->maplength)) {
		return -1;
	}

	return 0;
}