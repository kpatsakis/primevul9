static void dump_summary (WavpackContext *wpc, char *name, FILE *dst)
{
    uint32_t channel_mask = (uint32_t) WavpackGetChannelMask (wpc);
    int num_channels = WavpackGetNumChannels (wpc);
    unsigned char md5_sum [16];
    char modes [80];

    fprintf (dst, "\n");

    if (name && *name != '-') {
        fprintf (dst, "file name:         %s%s\n", name, (WavpackGetMode (wpc) & MODE_WVC) ? " (+wvc)" : "");
        fprintf (dst, "file size:         %lld bytes\n", (long long) WavpackGetFileSize64 (wpc));
    }

    if ((WavpackGetQualifyMode (wpc) & QMODE_DSD_AUDIO) && !raw_pcm)
        fprintf (dst, "source:            1-bit DSD at %u Hz\n", WavpackGetNativeSampleRate (wpc));
    else
        fprintf (dst, "source:            %d-bit %s at %u Hz\n", WavpackGetBitsPerSample (wpc),
            (WavpackGetMode (wpc) & MODE_FLOAT) ? "floats" : "ints",
            WavpackGetSampleRate (wpc));

    if (!channel_mask)
        strcpy (modes, "unassigned speakers");
    else if (num_channels == 1 && channel_mask == 0x4)
        strcpy (modes, "mono");
    else if (num_channels == 2 && channel_mask == 0x3)
        strcpy (modes, "stereo");
    else if (num_channels == 4 && channel_mask == 0x33)
        strcpy (modes, "quad");
    else if (num_channels == 6 && channel_mask == 0x3f)
        strcpy (modes, "5.1 surround");
    else if (num_channels == 6 && channel_mask == 0x60f)
        strcpy (modes, "5.1 surround side");
    else if (num_channels == 8 && channel_mask == 0x63f)
        strcpy (modes, "7.1 surround");
    else if (num_channels == 8 && channel_mask == 0x6000003f)
        strcpy (modes, "5.1 + stereo");
    else {
        int cc = num_channels, si = 0;
        uint32_t cm = channel_mask;

        modes [0] = 0;

        while (cc && cm) {
            if (cm & 1) {
                strcat (modes, si < 18 ? speakers [si] : "--");
                if (--cc)
                    strcat (modes, ",");
            }
            cm >>= 1;
            si++;
        }

        if (cc)
            strcat (modes, "...");
    }

    fprintf (dst, "channels:          %d (%s)\n", num_channels, modes);

    if (WavpackGetNumSamples64 (wpc) != -1) {
        double seconds = (double) WavpackGetNumSamples64 (wpc) / WavpackGetSampleRate (wpc);
        int minutes = (int) floor (seconds / 60.0);
        int hours = (int) floor (seconds / 3600.0);

        seconds -= minutes * 60.0;
        minutes -= (int)(hours * 60.0);

        fprintf (dst, "duration:          %d:%02d:%05.2f\n", hours, minutes, seconds);
    }

    modes [0] = 0;

    if (WavpackGetMode (wpc) & MODE_HYBRID)
        strcat (modes, "hybrid ");

    strcat (modes, (WavpackGetMode (wpc) & MODE_LOSSLESS) ? "lossless" : "lossy");

    if (WavpackGetMode (wpc) & MODE_FAST)
        strcat (modes, ", fast");
    else if (WavpackGetMode (wpc) & MODE_VERY_HIGH)
        strcat (modes, ", very high");
    else if (WavpackGetMode (wpc) & MODE_HIGH)
        strcat (modes, ", high");

    if (WavpackGetMode (wpc) & MODE_EXTRA) {
        strcat (modes, ", extra");

        if (WavpackGetMode (wpc) & MODE_XMODE) {
            char xmode[3] = "-0";

            xmode [1] = ((WavpackGetMode (wpc) & MODE_XMODE) >> 12) + '0';
            strcat (modes, xmode);
        }
    }

    if (WavpackGetMode (wpc) & MODE_SFX)
        strcat (modes, ", sfx");

    if (WavpackGetMode (wpc) & MODE_DNS)
        strcat (modes, ", dns");

    fprintf (dst, "modalities:        %s\n", modes);

    if (WavpackGetRatio (wpc) != 0.0) {
        fprintf (dst, "compression:       %.2f%%\n", 100.0 - (100 * WavpackGetRatio (wpc)));
        fprintf (dst, "ave bitrate:       %d kbps\n", (int) ((WavpackGetAverageBitrate (wpc, TRUE) + 500.0) / 1000.0));

        if (WavpackGetMode (wpc) & MODE_WVC)
            fprintf (dst, "ave lossy bitrate: %d kbps\n", (int) ((WavpackGetAverageBitrate (wpc, FALSE) + 500.0) / 1000.0));
    }

    if (WavpackGetVersion (wpc))
        fprintf (dst, "encoder version:   %d\n", WavpackGetVersion (wpc));

    if (WavpackGetMD5Sum (wpc, md5_sum)) {
        char md5_string [] = "00000000000000000000000000000000";
        int i;

        for (i = 0; i < 16; ++i)
            sprintf (md5_string + (i * 2), "%02x", md5_sum [i]);

        fprintf (dst, "original md5:      %s\n", md5_string);
    }

    if (summary > 1) {
        uint32_t header_bytes = WavpackGetWrapperBytes (wpc), trailer_bytes, i;
        unsigned char *header_data = WavpackGetWrapperData (wpc);
        char header_name [5];

        strcpy (header_name, "????");

        for (i = 0; i < 4 && i < header_bytes; ++i)
            if (header_data [i] >= 0x20 && header_data [i] <= 0x7f)
                header_name [i] = header_data [i];

        WavpackFreeWrapper (wpc);
        WavpackSeekTrailingWrapper (wpc);
        trailer_bytes = WavpackGetWrapperBytes (wpc);

        if (WavpackGetFileFormat (wpc) < NUM_FILE_FORMATS)
            fprintf (dst, "source format:     %s with '%s' extension\n",
                file_formats [WavpackGetFileFormat (wpc)].format_name, WavpackGetFileExtension (wpc));
        else
            fprintf (dst, "source format:     '%s' file\n", WavpackGetFileExtension (wpc));

        if (header_bytes && trailer_bytes) {
            unsigned char *trailer_data = WavpackGetWrapperData (wpc);
            char trailer_name [5];

            strcpy (trailer_name, "????");

            for (i = 0; i < 4 && i < trailer_bytes; ++i)
                if (trailer_data [i] >= 0x20 && trailer_data [i] <= 0x7f)
                    trailer_name [i] = trailer_data [i];

            fprintf (dst, "file wrapper:      %u + %u bytes (%s, %s)\n",
                header_bytes, trailer_bytes, header_name, trailer_name);
        }
        else if (header_bytes)
            fprintf (dst, "file wrapper:      %u byte %s header\n",
                header_bytes, header_name);
        else if (trailer_bytes)
            fprintf (dst, "file wrapper:      %u byte trailer only\n",
                trailer_bytes);
        else
            fprintf (dst, "file wrapper:      none stored\n");
    }

    if (WavpackGetMode (wpc) & MODE_VALID_TAG) {
        int ape_tag = WavpackGetMode (wpc) & MODE_APETAG;
        int num_binary_items = WavpackGetNumBinaryTagItems (wpc);
        int num_items = WavpackGetNumTagItems (wpc), i;
        char *spaces = "                  ";

        fprintf (dst, "\n%s tag items:   %d\n", ape_tag ? "APEv2" : "ID3v1", num_items + num_binary_items);

        for (i = 0; i < num_items; ++i) {
            int item_len, value_len, j;
            char *item, *value;

            item_len = WavpackGetTagItemIndexed (wpc, i, NULL, 0);
            item = malloc (item_len + 1);
            WavpackGetTagItemIndexed (wpc, i, item, item_len + 1);
            value_len = WavpackGetTagItem (wpc, item, NULL, 0);
            value = malloc (value_len * 2 + 1);
            WavpackGetTagItem (wpc, item, value, value_len + 1);

            fprintf (dst, "%s:%s", item, strlen (item) < strlen (spaces) ? spaces + strlen (item) : " ");

            if (ape_tag) {
                for (j = 0; j < value_len; ++j)
                    if (!value [j])
                        value [j] = '\\';

                if (strchr (value, '\n'))
                    fprintf (dst, "%d-byte multi-line text string\n", value_len);
                else {
                    dump_UTF8_string (value, dst);
                    fprintf (dst, "\n");
                }
            }
            else
                fprintf (dst, "%s\n", value);

            free (value);
            free (item);
        }

        for (i = 0; i < num_binary_items; ++i) {
            int item_len, value_len;
            char *item, fname [256];

            item_len = WavpackGetBinaryTagItemIndexed (wpc, i, NULL, 0);
            item = malloc (item_len + 1);
            WavpackGetBinaryTagItemIndexed (wpc, i, item, item_len + 1);
            value_len = dump_tag_item_to_file (wpc, item, NULL, fname);
            fprintf (dst, "%s:%s", item, strlen (item) < strlen (spaces) ? spaces + strlen (item) : " ");

            if (filespec_ext (fname))
                fprintf (dst, "%d-byte binary item (%s)\n", value_len, filespec_ext (fname)+1);
            else
                fprintf (dst, "%d-byte binary item\n", value_len);

#if 0   // debug binary tag reading
            {
                char md5_string [] = "00000000000000000000000000000000";
                unsigned char md5_result [16];
                MD5_CTX md5_context;
                char *value;
                int i, j;

                MD5_Init (&md5_context);
                value_len = WavpackGetBinaryTagItem (wpc, item, NULL, 0);
                value = malloc (value_len);
                value_len = WavpackGetBinaryTagItem (wpc, item, value, value_len);

                for (i = 0; i < value_len; ++i)
                    if (!value [i]) {
                        MD5_Update (&md5_context, (unsigned char *) value + i + 1, value_len - i - 1);
                        MD5_Final (md5_result, &md5_context);
                        for (j = 0; j < 16; ++j)
                            sprintf (md5_string + (j * 2), "%02x", md5_result [j]);
                        fprintf (dst, "    %d byte string >>%s<<\n", i, value);
                        fprintf (dst, "    %d bytes binary data >>%s<<\n", value_len - i - 1, md5_string);
                        break;
                    }

                if (i == value_len)
                    fprintf (dst, "    no NULL found in binary value (or value not readable)\n");

                free (value);
            }
#endif
            free (item);
        }
    }
}