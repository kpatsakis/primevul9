static int process_metadata (WavpackContext *wpc, WavpackMetadata *wpmd)
{
    WavpackStream *wps = wpc->streams [wpc->current_stream];

    switch (wpmd->id) {
        case ID_DUMMY:
            return TRUE;

        case ID_DECORR_TERMS:
            return read_decorr_terms (wps, wpmd);

        case ID_DECORR_WEIGHTS:
            return read_decorr_weights (wps, wpmd);

        case ID_DECORR_SAMPLES:
            return read_decorr_samples (wps, wpmd);

        case ID_ENTROPY_VARS:
            return read_entropy_vars (wps, wpmd);

        case ID_HYBRID_PROFILE:
            return read_hybrid_profile (wps, wpmd);

        case ID_SHAPING_WEIGHTS:
            return read_shaping_info (wps, wpmd);

        case ID_FLOAT_INFO:
            return read_float_info (wps, wpmd);

        case ID_INT32_INFO:
            return read_int32_info (wps, wpmd);

        case ID_CHANNEL_INFO:
            return read_channel_info (wpc, wpmd);

        case ID_CHANNEL_IDENTITIES:
            return read_channel_identities (wpc, wpmd);

        case ID_CONFIG_BLOCK:
            return read_config_info (wpc, wpmd);

        case ID_NEW_CONFIG_BLOCK:
            return read_new_config_info (wpc, wpmd);

        case ID_SAMPLE_RATE:
            return read_sample_rate (wpc, wpmd);

        case ID_WV_BITSTREAM:
            return init_wv_bitstream (wps, wpmd);

        case ID_WVC_BITSTREAM:
            return init_wvc_bitstream (wps, wpmd);

        case ID_WVX_BITSTREAM:
            return init_wvx_bitstream (wps, wpmd);

        case ID_DSD_BLOCK:
#ifdef ENABLE_DSD
            return init_dsd_block (wpc, wpmd);
#else
            strcpy (wpc->error_message, "not configured to handle DSD WavPack files!");
            return FALSE;
#endif

        case ID_ALT_HEADER: case ID_ALT_TRAILER:
            if (!(wpc->open_flags & OPEN_ALT_TYPES))
                return TRUE;

        case ID_RIFF_HEADER: case ID_RIFF_TRAILER:
            return read_wrapper_data (wpc, wpmd);

        case ID_ALT_MD5_CHECKSUM:
            if (!(wpc->open_flags & OPEN_ALT_TYPES))
                return TRUE;

        case ID_MD5_CHECKSUM:
            if (wpmd->byte_length == 16) {
                memcpy (wpc->config.md5_checksum, wpmd->data, 16);
                wpc->config.flags |= CONFIG_MD5_CHECKSUM;
                wpc->config.md5_read = 1;
            }

            return TRUE;

        case ID_ALT_EXTENSION:
            if (wpmd->byte_length && wpmd->byte_length < sizeof (wpc->file_extension)) {
                int i, j;

                for (i = j = 0; i < wpmd->byte_length; ++i)
                    if (isalnum (((char *) wpmd->data) [i]))
                        wpc->file_extension [j++] = ((char *) wpmd->data) [i];

                wpc->file_extension [j] = 0;
            }

            return TRUE;

        // we don't actually verify the checksum here (it's done right after the
        // block is read), but it's a good indicator of version 5 files

        case ID_BLOCK_CHECKSUM:
            wpc->version_five = 1;
            return TRUE;

        default:
            return (wpmd->id & ID_OPTIONAL_DATA) ? TRUE : FALSE;
    }
}