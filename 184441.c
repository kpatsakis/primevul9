static int unpack_dsd_audio (WavpackContext *wpc, FILE *outfile, int qmode, unsigned char *md5_digest, int64_t *sample_count)
{
    unsigned char *output_buffer = NULL, *new_channel_order = NULL;
    int num_channels = WavpackGetNumChannels (wpc), result = WAVPACK_NO_ERROR;
    int64_t until_samples_total = *sample_count, total_unpacked_samples = 0;
    uint32_t output_buffer_size = 0, bcount;
    double progress = -1.0;
    int32_t *temp_buffer;
    MD5_CTX md5_context;

    if (md5_digest)
        MD5_Init (&md5_context);

    output_buffer_size = DSD_BLOCKSIZE * num_channels;
    output_buffer = malloc (output_buffer_size);

    if (!output_buffer) {
        error_line ("can't allocate buffer for decoding!");
        WavpackCloseFile (wpc);
        return WAVPACK_HARD_ERROR;
    }

    if (qmode & QMODE_REORDERED_CHANS) {
        int layout = WavpackGetChannelLayout (wpc, NULL), i;

        if ((layout & 0xff) <= num_channels) {
            new_channel_order = malloc (num_channels);

            for (i = 0; i < num_channels; ++i)
                new_channel_order [i] = i;

            WavpackGetChannelLayout (wpc, new_channel_order);
        }
    }

    temp_buffer = malloc (DSD_BLOCKSIZE * num_channels * sizeof (temp_buffer [0]));

    while (result == WAVPACK_NO_ERROR) {
        uint32_t samples_to_unpack = DSD_BLOCKSIZE, samples_unpacked;

        if (until_samples_total && samples_to_unpack > until_samples_total - total_unpacked_samples)
            samples_to_unpack = (uint32_t) (until_samples_total - total_unpacked_samples);

        samples_unpacked = WavpackUnpackSamples (wpc, temp_buffer, samples_to_unpack);
        total_unpacked_samples += samples_unpacked;

        if (new_channel_order)
            unreorder_channels (temp_buffer, new_channel_order, num_channels, samples_unpacked);

        if (samples_unpacked) {
            unsigned char *dptr = output_buffer;
            int32_t *sptr = temp_buffer;

            if (qmode & QMODE_DSD_IN_BLOCKS) {
                int cc = num_channels;

                while (cc--) {
                    uint32_t si;

                    for (si = 0; si < DSD_BLOCKSIZE; si++, sptr += num_channels)
                        if (si < samples_unpacked)
                            *dptr++ = (qmode & QMODE_DSD_LSB_FIRST) ? bit_reverse_table [*sptr & 0xff] : *sptr;
                        else
                            *dptr++ = 0;

                    sptr -= (DSD_BLOCKSIZE * num_channels) - 1;
                }

                samples_unpacked = DSD_BLOCKSIZE;   // make sure we MD5 and write the whole block even if partial (last)
            }
            else {
                int scount = samples_unpacked * num_channels;

                while (scount--)
                    *dptr++ = *sptr++;
            }

            if (md5_digest)
                MD5_Update (&md5_context, output_buffer, samples_unpacked * num_channels);

            if (outfile && (!DoWriteFile (outfile, output_buffer, samples_unpacked * num_channels, &bcount) ||
                bcount != samples_unpacked * num_channels)) {
                    error_line ("can't write .WAV data, disk probably full!");
                    DoTruncateFile (outfile);
                    result = WAVPACK_HARD_ERROR;
                    break;
                }
        }
        else
            break;

        if (check_break ()) {
#if defined(_WIN32)
            fprintf (stderr, "^C\n");
#else
            fprintf (stderr, "\n");
#endif
            fflush (stderr);
            DoTruncateFile (outfile);
            result = WAVPACK_SOFT_ERROR;
            break;
        }

        if (WavpackGetProgress (wpc) != -1.0 &&
            progress != floor (WavpackGetProgress (wpc) * 100.0 + 0.5)) {
                int nobs = progress == -1.0;

                progress = WavpackGetProgress (wpc);
                display_progress (progress);
                progress = floor (progress * 100.0 + 0.5);

                if (!quiet_mode) {
                    fprintf (stderr, "%s%3d%% done...",
                        nobs ? " " : "\b\b\b\b\b\b\b\b\b\b\b\b", (int) progress);
                    fflush (stderr);
                }
        }
    }

    if (new_channel_order)
        free (new_channel_order);

    if (md5_digest)
        MD5_Final (md5_digest, &md5_context);

    free (temp_buffer);

    if (output_buffer)
        free (output_buffer);

    *sample_count = total_unpacked_samples;
    return result;
}