static int match_wvc_header (WavpackHeader *wv_hdr, WavpackHeader *wvc_hdr)
{
    if (GET_BLOCK_INDEX (*wv_hdr) == GET_BLOCK_INDEX (*wvc_hdr) &&
        wv_hdr->block_samples == wvc_hdr->block_samples) {
            int wvi = 0, wvci = 0;

            if (wv_hdr->flags == wvc_hdr->flags)
                return 0;

            if (wv_hdr->flags & INITIAL_BLOCK)
                wvi -= 1;

            if (wv_hdr->flags & FINAL_BLOCK)
                wvi += 1;

            if (wvc_hdr->flags & INITIAL_BLOCK)
                wvci -= 1;

            if (wvc_hdr->flags & FINAL_BLOCK)
                wvci += 1;

            return (wvci - wvi < 0) ? 1 : -1;
        }

    if ((GET_BLOCK_INDEX (*wvc_hdr) - GET_BLOCK_INDEX (*wv_hdr)) & 0x8000000000LL)
        return 1;
    else
        return -1;
}