static int unpack_audio (WavpackContext *wpc, FILE *outfile, int qmode, unsigned char *md5_digest, int64_t *sample_count)
{
    unsigned char *output_buffer = NULL, *output_pointer = NULL, *new_channel_order = NULL;
    int bps = WavpackGetBytesPerSample (wpc), num_channels = WavpackGetNumChannels (wpc);
    int64_t until_samples_total = *sample_count, total_unpacked_samples = 0;
    int bytes_per_sample = bps * num_channels, result = WAVPACK_NO_ERROR;
    uint32_t output_buffer_size = 0, bcount;
    double progress = -1.0;
    int32_t *temp_buffer;
    MD5_CTX md5_context;

    if (md5_digest)
        MD5_Init (&md5_context);

    if (outfile) {
        if (outbuf_k)
            output_buffer_size = outbuf_k * 1024;
        else
            output_buffer_size = 1024 * 256;

        output_pointer = output_buffer = malloc (output_buffer_size);

        if (!output_buffer) {
            error_line ("can't allocate buffer for decoding!");
            WavpackCloseFile (wpc);
            return WAVPACK_HARD_ERROR;
        }
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

    temp_buffer = malloc (TEMP_BUFFER_SAMPLES * num_channels * sizeof (temp_buffer [0]));

    while (result == WAVPACK_NO_ERROR) {
        uint32_t samples_to_unpack, samples_unpacked;

        if (output_buffer) {
            samples_to_unpack = (output_buffer_size - (uint32_t)(output_pointer - output_buffer)) / bytes_per_sample;

            if (samples_to_unpack > TEMP_BUFFER_SAMPLES)
                samples_to_unpack = TEMP_BUFFER_SAMPLES;
        }
        else
            samples_to_unpack = TEMP_BUFFER_SAMPLES;

        if (until_samples_total && samples_to_unpack > until_samples_total - total_unpacked_samples)
            samples_to_unpack = (uint32_t) (until_samples_total - total_unpacked_samples);

        samples_unpacked = WavpackUnpackSamples (wpc, temp_buffer, samples_to_unpack);
        total_unpacked_samples += samples_unpacked;

        if (new_channel_order)
            unreorder_channels (temp_buffer, new_channel_order, num_channels, samples_unpacked);

        if (output_buffer) {
            if (samples_unpacked)
                output_pointer = store_samples (output_pointer, temp_buffer, qmode, bps, samples_unpacked * num_channels);

            if (!samples_unpacked || (output_buffer_size - (output_pointer - output_buffer)) < (uint32_t) bytes_per_sample) {
                if (!DoWriteFile (outfile, output_buffer, (uint32_t)(output_pointer - output_buffer), &bcount) ||
                    bcount != output_pointer - output_buffer) {
                        error_line ("can't write .WAV data, disk probably full!");
                        DoTruncateFile (outfile);
                        result = WAVPACK_HARD_ERROR;
                        break;
                }

                output_pointer = output_buffer;
            }
        }

        if (md5_digest && samples_unpacked) {
            store_samples (temp_buffer, temp_buffer, qmode, bps, samples_unpacked * num_channels);
            MD5_Update (&md5_context, (unsigned char *) temp_buffer, bps * samples_unpacked * num_channels);
        }

        if (!samples_unpacked)
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