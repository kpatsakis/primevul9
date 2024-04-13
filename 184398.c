static void dump_file_item (WavpackContext *wpc, char *str, int item_id)
{
    unsigned char md5_sum [16];

    switch (item_id) {
        case 1:
            sprintf (str + strlen (str), "%d", raw_pcm ? WavpackGetSampleRate (wpc) : WavpackGetNativeSampleRate (wpc));
            break;

        case 2:
            sprintf (str + strlen (str), "%d", ((WavpackGetQualifyMode (wpc) & QMODE_DSD_AUDIO) && !raw_pcm) ? 1 : WavpackGetBitsPerSample (wpc));
            break;

        case 3:
            sprintf (str + strlen (str), "%s", (WavpackGetMode (wpc) & MODE_FLOAT) ? "float" : "int");
            break;

        case 4:
            sprintf (str + strlen (str), "%d", WavpackGetNumChannels (wpc));
            break;

        case 5:
            sprintf (str + strlen (str), "0x%x", WavpackGetChannelMask (wpc));
            break;

        case 6:
            if (WavpackGetNumSamples64 (wpc) != -1)
                sprintf (str + strlen (str), "%lld",
                    (long long int) WavpackGetNumSamples64 (wpc) * (WavpackGetQualifyMode (wpc) & QMODE_DSD_AUDIO ? 8 : 1));

            break;

        case 7:
            if (WavpackGetMD5Sum (wpc, md5_sum)) {
                char md5_string [] = "00000000000000000000000000000000";
                int i;

                for (i = 0; i < 16; ++i)
                    sprintf (md5_string + (i * 2), "%02x", md5_sum [i]);

                sprintf (str + strlen (str), "%s", md5_string);
            }

            break;

        case 8:
            sprintf (str + strlen (str), "%d", WavpackGetVersion (wpc));
            break;

        case 9:
            sprintf (str + strlen (str), "0x%x", WavpackGetMode (wpc));
            break;

        default:
            break;
    }
}