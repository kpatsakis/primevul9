int read_wvc_block (WavpackContext *wpc)
{
    WavpackStream *wps = wpc->streams [wpc->current_stream];
    int64_t bcount, file2pos;
    WavpackHeader orig_wphdr;
    WavpackHeader wphdr;
    int compare_result;

    while (1) {
        file2pos = wpc->reader->get_pos (wpc->wvc_in);
        bcount = read_next_header (wpc->reader, wpc->wvc_in, &wphdr);

        if (bcount == (uint32_t) -1) {
            wps->wvc_skip = TRUE;
            wpc->crc_errors++;
            return FALSE;
        }

        memcpy (&orig_wphdr, &wphdr, 32);       // save original header for verify step

        if (wpc->open_flags & OPEN_STREAMING)
            SET_BLOCK_INDEX (wphdr, wps->sample_index = 0);
        else
            SET_BLOCK_INDEX (wphdr, GET_BLOCK_INDEX (wphdr) - wpc->initial_index);

        if (wphdr.flags & INITIAL_BLOCK)
            wpc->file2pos = file2pos + bcount;

        compare_result = match_wvc_header (&wps->wphdr, &wphdr);

        if (!compare_result) {
            wps->block2buff = (unsigned char *)malloc (wphdr.ckSize + 8);
	    if (!wps->block2buff)
	        return FALSE;

            if (wpc->reader->read_bytes (wpc->wvc_in, wps->block2buff + 32, wphdr.ckSize - 24) !=
                wphdr.ckSize - 24) {
                    free (wps->block2buff);
                    wps->block2buff = NULL;
                    wps->wvc_skip = TRUE;
                    wpc->crc_errors++;
                    return FALSE;
            }

            memcpy (wps->block2buff, &orig_wphdr, 32);

            // don't use corrupt blocks
            if (!WavpackVerifySingleBlock (wps->block2buff, !(wpc->open_flags & OPEN_NO_CHECKSUM))) {
                free (wps->block2buff);
                wps->block2buff = NULL;
                wps->wvc_skip = TRUE;
                wpc->crc_errors++;
                return TRUE;
            }

            wps->wvc_skip = FALSE;
            memcpy (wps->block2buff, &wphdr, 32);
            memcpy (&wps->wphdr, &wphdr, 32);
            return TRUE;
        }
        else if (compare_result == -1) {
            wps->wvc_skip = TRUE;
            wpc->reader->set_pos_rel (wpc->wvc_in, -32, SEEK_CUR);
            wpc->crc_errors++;
            return TRUE;
        }
    }
}