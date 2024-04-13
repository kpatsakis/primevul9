int unpack_init (WavpackContext *wpc)
{
    WavpackStream *wps = wpc->streams [wpc->current_stream];
    unsigned char *blockptr, *block2ptr;
    WavpackMetadata wpmd;

    wps->num_terms = 0;
    wps->mute_error = FALSE;
    wps->crc = wps->crc_x = 0xffffffff;
    wps->dsd.ready = 0;
    CLEAR (wps->wvbits);
    CLEAR (wps->wvcbits);
    CLEAR (wps->wvxbits);
    CLEAR (wps->decorr_passes);
    CLEAR (wps->dc);
    CLEAR (wps->w);

    if (!(wps->wphdr.flags & MONO_FLAG) && wpc->config.num_channels && wps->wphdr.block_samples &&
        (wpc->reduced_channels == 1 || wpc->config.num_channels == 1)) {
            wps->mute_error = TRUE;
            return FALSE;
    }

    if ((wps->wphdr.flags & UNKNOWN_FLAGS) || (wps->wphdr.flags & MONO_DATA) == MONO_DATA) {
        wps->mute_error = TRUE;
        return FALSE;
    }

    blockptr = wps->blockbuff + sizeof (WavpackHeader);

    while (read_metadata_buff (&wpmd, wps->blockbuff, &blockptr))
        if (!process_metadata (wpc, &wpmd)) {
            wps->mute_error = TRUE;
            return FALSE;
        }

    if (wps->wphdr.block_samples && wpc->wvc_flag && wps->block2buff) {
        block2ptr = wps->block2buff + sizeof (WavpackHeader);

        while (read_metadata_buff (&wpmd, wps->block2buff, &block2ptr))
            if (!process_metadata (wpc, &wpmd)) {
                wps->mute_error = TRUE;
                return FALSE;
            }
    }

    if (wps->wphdr.block_samples && ((wps->wphdr.flags & DSD_FLAG) ? !wps->dsd.ready : !bs_is_open (&wps->wvbits))) {
        if (bs_is_open (&wps->wvcbits))
            strcpy (wpc->error_message, "can't unpack correction files alone!");

        wps->mute_error = TRUE;
        return FALSE;
    }

    if (wps->wphdr.block_samples && !bs_is_open (&wps->wvxbits)) {
        if ((wps->wphdr.flags & INT32_DATA) && wps->int32_sent_bits)
            wpc->lossy_blocks = TRUE;

        if ((wps->wphdr.flags & FLOAT_DATA) &&
            wps->float_flags & (FLOAT_EXCEPTIONS | FLOAT_ZEROS_SENT | FLOAT_SHIFT_SENT | FLOAT_SHIFT_SAME))
                wpc->lossy_blocks = TRUE;
    }

    if (wps->wphdr.block_samples)
        wps->sample_index = GET_BLOCK_INDEX (wps->wphdr);

    return TRUE;
}