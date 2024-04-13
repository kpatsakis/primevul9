WavpackContext *WavpackOpenFileInputEx64 (WavpackStreamReader64 *reader, void *wv_id, void *wvc_id, char *error, int flags, int norm_offset)
{
    WavpackContext *wpc = (WavpackContext *)malloc (sizeof (WavpackContext));
    WavpackStream *wps;
    int num_blocks = 0;
    unsigned char first_byte;
    uint32_t bcount;

    if (!wpc) {
        if (error) strcpy (error, "can't allocate memory");
        return NULL;
    }

    CLEAR (*wpc);
    wpc->wv_in = wv_id;
    wpc->wvc_in = wvc_id;
    wpc->reader = reader;
    wpc->total_samples = -1;
    wpc->norm_offset = norm_offset;
    wpc->max_streams = OLD_MAX_STREAMS;     // use this until overwritten with actual number
    wpc->open_flags = flags;

    wpc->filelen = wpc->reader->get_length (wpc->wv_in);

#ifndef NO_TAGS
    if ((flags & (OPEN_TAGS | OPEN_EDIT_TAGS)) && wpc->reader->can_seek (wpc->wv_in)) {
        load_tag (wpc);
        wpc->reader->set_pos_abs (wpc->wv_in, 0);

        if ((flags & OPEN_EDIT_TAGS) && !editable_tag (&wpc->m_tag)) {
            if (error) strcpy (error, "can't edit tags located at the beginning of files!");
            return WavpackCloseFile (wpc);
        }
    }
#endif

    if (wpc->reader->read_bytes (wpc->wv_in, &first_byte, 1) != 1) {
        if (error) strcpy (error, "can't read all of WavPack file!");
        return WavpackCloseFile (wpc);
    }

    wpc->reader->push_back_byte (wpc->wv_in, first_byte);

    if (first_byte == 'R') {
#ifdef ENABLE_LEGACY
        return open_file3 (wpc, error);
#else
        if (error) strcpy (error, "this legacy WavPack file is deprecated, use version 4.80.0 to transcode");
        return WavpackCloseFile (wpc);
#endif
    }

    wpc->streams = (WavpackStream **)(malloc ((wpc->num_streams = 1) * sizeof (wpc->streams [0])));
    if (!wpc->streams) {
        if (error) strcpy (error, "can't allocate memory");
        return WavpackCloseFile (wpc);
    }

    wpc->streams [0] = wps = (WavpackStream *)malloc (sizeof (WavpackStream));
    if (!wps) {
        if (error) strcpy (error, "can't allocate memory");
        return WavpackCloseFile (wpc);
    }
    CLEAR (*wps);

    while (!wps->wphdr.block_samples) {

        wpc->filepos = wpc->reader->get_pos (wpc->wv_in);
        bcount = read_next_header (wpc->reader, wpc->wv_in, &wps->wphdr);

        if (bcount == (uint32_t) -1 ||
            (!wps->wphdr.block_samples && num_blocks++ > 16)) {
                if (error) strcpy (error, "not compatible with this version of WavPack file!");
                return WavpackCloseFile (wpc);
        }

        wpc->filepos += bcount;
        wps->blockbuff = (unsigned char *)malloc (wps->wphdr.ckSize + 8);
        if (!wps->blockbuff) {
            if (error) strcpy (error, "can't allocate memory");
            return WavpackCloseFile (wpc);
        }
        memcpy (wps->blockbuff, &wps->wphdr, 32);

        if (wpc->reader->read_bytes (wpc->wv_in, wps->blockbuff + 32, wps->wphdr.ckSize - 24) != wps->wphdr.ckSize - 24) {
            if (error) strcpy (error, "can't read all of WavPack file!");
            return WavpackCloseFile (wpc);
        }

        // if block does not verify, flag error, free buffer, and continue
        if (!WavpackVerifySingleBlock (wps->blockbuff, !(flags & OPEN_NO_CHECKSUM))) {
            wps->wphdr.block_samples = 0;
            free (wps->blockbuff);
            wps->blockbuff = NULL;
            wpc->crc_errors++;
            continue;
        }

        wps->init_done = FALSE;

        if (wps->wphdr.block_samples) {
            if (flags & OPEN_STREAMING)
                SET_BLOCK_INDEX (wps->wphdr, 0);
            else if (wpc->total_samples == -1) {
                if (GET_BLOCK_INDEX (wps->wphdr) || GET_TOTAL_SAMPLES (wps->wphdr) == -1) {
                    wpc->initial_index = GET_BLOCK_INDEX (wps->wphdr);
                    SET_BLOCK_INDEX (wps->wphdr, 0);

                    if (wpc->reader->can_seek (wpc->wv_in)) {
                        int64_t final_index = -1;

                        seek_eof_information (wpc, &final_index, FALSE);

                        if (final_index != -1)
                            wpc->total_samples = final_index - wpc->initial_index;
                    }
                }
                else
                    wpc->total_samples = GET_TOTAL_SAMPLES (wps->wphdr);
            }
        }
        else if (wpc->total_samples == -1 && !GET_BLOCK_INDEX (wps->wphdr) && GET_TOTAL_SAMPLES (wps->wphdr))
            wpc->total_samples = GET_TOTAL_SAMPLES (wps->wphdr);

        if (wpc->wvc_in && wps->wphdr.block_samples && (wps->wphdr.flags & HYBRID_FLAG)) {
            unsigned char ch;

            if (wpc->reader->read_bytes (wpc->wvc_in, &ch, 1) == 1) {
                wpc->reader->push_back_byte (wpc->wvc_in, ch);
                wpc->file2len = wpc->reader->get_length (wpc->wvc_in);
                wpc->wvc_flag = TRUE;
            }
        }

        if (wpc->wvc_flag && !read_wvc_block (wpc)) {
            if (error) strcpy (error, "not compatible with this version of correction file!");
            return WavpackCloseFile (wpc);
        }

        if (!wps->init_done && !unpack_init (wpc)) {
            if (error) strcpy (error, wpc->error_message [0] ? wpc->error_message :
                "not compatible with this version of WavPack file!");

            return WavpackCloseFile (wpc);
        }

        if (!wps->wphdr.block_samples) {    // free blockbuff if we're going to loop again
            free (wps->blockbuff);
            wps->blockbuff = NULL;
        }

        wps->init_done = TRUE;
    }

    wpc->config.flags &= ~0xff;
    wpc->config.flags |= wps->wphdr.flags & 0xff;

    if (!wpc->config.num_channels) {
        wpc->config.num_channels = (wps->wphdr.flags & MONO_FLAG) ? 1 : 2;
        wpc->config.channel_mask = 0x5 - wpc->config.num_channels;
    }

    if ((flags & OPEN_2CH_MAX) && !(wps->wphdr.flags & FINAL_BLOCK))
        wpc->reduced_channels = (wps->wphdr.flags & MONO_FLAG) ? 1 : 2;

    if (wps->wphdr.flags & DSD_FLAG) {
#ifdef ENABLE_DSD
        if (flags & OPEN_DSD_NATIVE) {
            wpc->config.bytes_per_sample = 1;
            wpc->config.bits_per_sample = 8;
        }
        else if (flags & OPEN_DSD_AS_PCM) {
            wpc->decimation_context = decimate_dsd_init (wpc->reduced_channels ?
                wpc->reduced_channels : wpc->config.num_channels);

            wpc->config.bytes_per_sample = 3;
            wpc->config.bits_per_sample = 24;
        }
        else {
            if (error) strcpy (error, "not configured to handle DSD WavPack files!");
            return WavpackCloseFile (wpc);
        }
#else
        if (error) strcpy (error, "not configured to handle DSD WavPack files!");
        return WavpackCloseFile (wpc);
#endif
    }
    else {
        wpc->config.bytes_per_sample = (wps->wphdr.flags & BYTES_STORED) + 1;
        wpc->config.float_norm_exp = wps->float_norm_exp;

        wpc->config.bits_per_sample = (wpc->config.bytes_per_sample * 8) -
            ((wps->wphdr.flags & SHIFT_MASK) >> SHIFT_LSB);
    }

    if (!wpc->config.sample_rate) {
        if (!wps->wphdr.block_samples || (wps->wphdr.flags & SRATE_MASK) == SRATE_MASK)
            wpc->config.sample_rate = 44100;
        else
            wpc->config.sample_rate = sample_rates [(wps->wphdr.flags & SRATE_MASK) >> SRATE_LSB];
    }

    return wpc;
}