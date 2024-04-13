static int unpack_file (char *infilename, char *outfilename, int add_extension)
{
    int64_t skip_sample_index = 0, until_samples_total = 0, total_unpacked_samples = 0;
    int result = WAVPACK_NO_ERROR, md5_diff = FALSE, created_riff_header = FALSE;
    int input_qmode, output_qmode = 0, input_format, output_format = 0;
    int open_flags = 0, padding_bytes = 0, num_channels, wvc_mode;
    unsigned char md5_unpacked [16];
    char *outfilename_temp = NULL;
    char *extension = NULL;
    WavpackContext *wpc;
    uint32_t bcount;
    char error [80];
    FILE *outfile;
    double dtime;

#if defined(_WIN32)
    struct __timeb64 time1, time2;
#else
    struct timeval time1, time2;
    struct timezone timez;
#endif

    // use library to open WavPack file

#ifdef _WIN32
    open_flags |= OPEN_FILE_UTF8;
#endif

    if (normalize_floats)
        open_flags |= OPEN_NORMALIZE;

    if ((outfilename && !raw_decode && !blind_decode && !format_specified &&
        !skip.value_is_valid && !until.value_is_valid) || summary > 1)
            open_flags |= OPEN_WRAPPER;

    if (blind_decode)
        open_flags |= OPEN_STREAMING | OPEN_NO_CHECKSUM;

    if (!ignore_wvc)
        open_flags |= OPEN_WVC;

    if (summary > 1 || num_tag_extractions || tag_extract_stdout)
        open_flags |= OPEN_TAGS;

    if ((format_specified && decode_format != WP_FORMAT_DFF && decode_format != WP_FORMAT_DSF) || raw_pcm)
        open_flags |= OPEN_DSD_AS_PCM | OPEN_ALT_TYPES;
    else
        open_flags |= OPEN_DSD_NATIVE | OPEN_ALT_TYPES;

    wpc = WavpackOpenFileInput (infilename, error, open_flags, 0);

    if (!wpc) {
        error_line (error);
        return WAVPACK_SOFT_ERROR;
    }

    if (add_extension) {
        if (raw_decode)
            extension = "raw";
        else if (format_specified)
            extension = file_formats [decode_format].default_extension;
        else
            extension = WavpackGetFileExtension (wpc);
    }

    wvc_mode = WavpackGetMode (wpc) & MODE_WVC;
    num_channels = WavpackGetNumChannels (wpc);
    input_qmode = WavpackGetQualifyMode (wpc);
    input_format = WavpackGetFileFormat (wpc);

    // Based on what output format the user specified on the command-line (if any) and what we can
    // tell about the file, decide on how we are going to format the output. Note that the last
    // couple of cases refer to the situation where the file is a format we don't understand,
    // which can't really happen now, but could happen some time in the future when either a new
    // format is added or when someone creates a "custom" format and that file meets a standard
    // decoder.

    if (raw_decode) {                                   // case 1: user specified raw decode
        if ((input_qmode & QMODE_DSD_AUDIO) && !raw_pcm)
            output_qmode = QMODE_DSD_MSB_FIRST;
        else
            output_qmode = 0;
    }
    else if (format_specified) {                        // case 2: user specified an output format
        switch (decode_format) {
            case WP_FORMAT_CAF:
                output_qmode = QMODE_SIGNED_BYTES | (caf_be ? QMODE_BIG_ENDIAN : 0) | (input_qmode & QMODE_REORDERED_CHANS);
                output_format = decode_format;
                break;

            case WP_FORMAT_WAV:
            case WP_FORMAT_W64:
                output_format = decode_format;
                output_qmode = 0;
                break;

            case WP_FORMAT_DFF:
            case WP_FORMAT_DSF:
                if (!(input_qmode & QMODE_DSD_AUDIO)) {
                    error_line ("can't export PCM source to DSD file!");
                    WavpackCloseFile (wpc);
                    return WAVPACK_SOFT_ERROR;
                }

                if (decode_format == WP_FORMAT_DSF)
                    output_qmode = QMODE_DSD_LSB_FIRST | QMODE_DSD_IN_BLOCKS;
                else
                    output_qmode = QMODE_DSD_MSB_FIRST;

                output_format = decode_format;
                break;
        }
    }
    else if (input_format < NUM_FILE_FORMATS) {         // case 3: user specified nothing, and this is a format we know about
        output_format = input_format;                   //   (obviously this is the most common situation)
        output_qmode = input_qmode;
    }
    else if (!WavpackGetWrapperBytes (wpc) ||           // case 4: unknown format and no wrapper present or extracting section
        skip.value_is_valid || until.value_is_valid) {  //   so we must override to a known format & extension

        if (input_qmode & QMODE_DSD_AUDIO) {
            output_format = WP_FORMAT_DFF;
            output_qmode = QMODE_DSD_MSB_FIRST;
        }
        else {
            output_format = WP_FORMAT_WAV;
            output_qmode = 0;
        }

        extension = file_formats [output_format].default_extension;
    }
    else                                                // case 5: unknown format, but wrapper is present and we're doing
        output_qmode = input_qmode;                     //   the whole file, so we don't have to understand the format

    if (skip.value_is_valid) {
        if (skip.value_is_time)
            skip_sample_index = (int64_t) (skip.value * WavpackGetSampleRate (wpc));
        else
            skip_sample_index = (int64_t) skip.value;

        if (skip.value_is_relative == -1) {
            if (WavpackGetNumSamples64 (wpc) == -1) {
                error_line ("can't use negative relative --skip command with files of unknown length!");
                WavpackCloseFile (wpc);
                return WAVPACK_SOFT_ERROR;
            }

            if (skip_sample_index < WavpackGetNumSamples64 (wpc))
                skip_sample_index = WavpackGetNumSamples64 (wpc) - skip_sample_index;
            else
                skip_sample_index = 0;
        }

        if (skip_sample_index && !WavpackSeekSample64 (wpc, skip_sample_index)) {
            error_line ("can't seek to specified --skip point!");
            WavpackCloseFile (wpc);
            return WAVPACK_SOFT_ERROR;
        }

        if (WavpackGetNumSamples64 (wpc) != -1)
            until_samples_total = WavpackGetNumSamples64 (wpc) - skip_sample_index;
    }

    if (until.value_is_valid) {
        double until_sample_index = until.value_is_time ? until.value * WavpackGetSampleRate (wpc) : until.value;

        if (until.value_is_relative == -1) {
            if (WavpackGetNumSamples64 (wpc) == -1) {
                error_line ("can't use negative relative --until command with files of unknown length!");
                WavpackCloseFile (wpc);
                return WAVPACK_SOFT_ERROR;
            }

            if (until_sample_index + skip_sample_index < WavpackGetNumSamples64 (wpc))
                until_samples_total = (int64_t) (WavpackGetNumSamples64 (wpc) - until_sample_index - skip_sample_index);
            else
                until_samples_total = 0;
        }
        else {
            if (until.value_is_relative == 1)
                until_samples_total = (int64_t) until_sample_index;
            else if (until_sample_index > skip_sample_index)
                until_samples_total = (int64_t) (until_sample_index - skip_sample_index);
            else
                until_samples_total = 0;

            if (WavpackGetNumSamples64 (wpc) != -1 &&
                skip_sample_index + until_samples_total > WavpackGetNumSamples64 (wpc))
                    until_samples_total = WavpackGetNumSamples64 (wpc) - skip_sample_index;
        }

        if (!until_samples_total) {
            error_line ("--until command results in no samples to decode!");
            WavpackCloseFile (wpc);
            return WAVPACK_SOFT_ERROR;
        }
    }

    if (file_info)
        dump_file_info (wpc, infilename, stdout, file_info - 1);
    else if (summary)
        dump_summary (wpc, infilename, stdout);
    else if (tag_extract_stdout) {
        if (!dump_tag_item_to_file (wpc, tag_extract_stdout, stdout, NULL)) {
            error_line ("tag \"%s\" not found!", tag_extract_stdout);
            WavpackCloseFile (wpc);
            return WAVPACK_SOFT_ERROR;
        }
    }
    else if (num_tag_extractions && outfilename && *outfilename != '-' && filespec_name (outfilename)) {
        result = do_tag_extractions (wpc, outfilename);

        if (result != WAVPACK_NO_ERROR) {
            WavpackCloseFile (wpc);
            return result;
        }
    }

    if (no_audio_decode) {
        WavpackCloseFile (wpc);
        return WAVPACK_NO_ERROR;
    }

    if (outfilename) {
        if (*outfilename != '-' && add_extension) {
            strcat (outfilename, ".");
            strcat (outfilename, extension);
        }

        if ((outfile = open_output_file (outfilename, &outfilename_temp)) == NULL) {
            WavpackCloseFile (wpc);
            return WAVPACK_SOFT_ERROR;
        }
        else if (*outfilename == '-') {
            if (!quiet_mode) {
                fprintf (stderr, "unpacking %s%s to stdout,", *infilename == '-' ?
                    "stdin" : FN_FIT (infilename), wvc_mode ? " (+.wvc)" : "");
                fflush (stderr);
            }
        }
        else if (!quiet_mode) {
            fprintf (stderr, "restoring %s,", FN_FIT (outfilename));
            fflush (stderr);
        }
    }
    else {      // in verify only mode we don't worry about headers
        outfile = NULL;

        if (!quiet_mode) {
            fprintf (stderr, "verifying %s%s,", *infilename == '-' ? "stdin" :
                FN_FIT (infilename), wvc_mode ? " (+.wvc)" : "");
            fflush (stderr);
        }
    }

#if defined(_WIN32)
    _ftime64 (&time1);
#else
    gettimeofday(&time1,&timez);
#endif

    if (outfile && !raw_decode) {
        if (until_samples_total) {
            if (!file_formats [output_format].WriteHeader (outfile, wpc, until_samples_total, output_qmode)) {
                DoTruncateFile (outfile);
                result = WAVPACK_HARD_ERROR;
            }
            else
                created_riff_header = TRUE;
        }
        else if (WavpackGetWrapperBytes (wpc)) {
            if (!DoWriteFile (outfile, WavpackGetWrapperData (wpc), WavpackGetWrapperBytes (wpc), &bcount) ||
                bcount != WavpackGetWrapperBytes (wpc)) {
                    error_line ("can't write .WAV data, disk probably full!");
                    DoTruncateFile (outfile);
                    result = WAVPACK_HARD_ERROR;
            }

            WavpackFreeWrapper (wpc);
        }
        else if (!file_formats [output_format].WriteHeader (outfile, wpc, WavpackGetNumSamples64 (wpc), output_qmode)) {
            DoTruncateFile (outfile);
            result = WAVPACK_HARD_ERROR;
        }
        else
            created_riff_header = TRUE;
    }

    total_unpacked_samples = until_samples_total;

    if (result == WAVPACK_NO_ERROR) {
        if (output_qmode & QMODE_DSD_AUDIO)
            result = unpack_dsd_audio (wpc, outfile, output_qmode, calc_md5 ? md5_unpacked : NULL, &total_unpacked_samples);
        else
            result = unpack_audio (wpc, outfile, output_qmode, calc_md5 ? md5_unpacked : NULL, &total_unpacked_samples);
    }

    // if the file format has chunk alignment requirements, and our data chunk does not align, write padding bytes here

    if (result == WAVPACK_NO_ERROR && outfile && !raw_decode && file_formats [output_format].chunk_alignment != 1) {
        int64_t data_chunk_bytes = total_unpacked_samples * num_channels * WavpackGetBytesPerSample (wpc);
        int alignment = file_formats [output_format].chunk_alignment;
        int bytes_over = (int)(data_chunk_bytes % alignment);
        int pcount = bytes_over ? alignment - bytes_over : 0;

        padding_bytes = pcount;

        while (pcount--)
            if (!DoWriteFile (outfile, "", 1, &bcount) || bcount != 1) {
                error_line ("can't write .WAV data, disk probably full!");
                DoTruncateFile (outfile);
                result = WAVPACK_HARD_ERROR;
            }
    }

    if (!check_break () && calc_md5) {
        char md5_string1 [] = "00000000000000000000000000000000";
        char md5_string2 [] = "00000000000000000000000000000000";
        unsigned char md5_original [16];
        int i;

        if (WavpackGetMD5Sum (wpc, md5_original)) {

            for (i = 0; i < 16; ++i)
                sprintf (md5_string1 + (i * 2), "%02x", md5_original [i]);

            error_line ("original md5:  %s", md5_string1);

            if (memcmp (md5_unpacked, md5_original, 16))
                md5_diff = TRUE;
        }

        for (i = 0; i < 16; ++i)
            sprintf (md5_string2 + (i * 2), "%02x", md5_unpacked [i]);

        error_line ("unpacked md5:  %s", md5_string2);
    }

    // this is where we append any trailing wrapper, assuming that we did not create the header
    // and that there is actually one stored that came from the original file

    if (outfile && result == WAVPACK_NO_ERROR && !created_riff_header && WavpackGetWrapperBytes (wpc)) {
        unsigned char *wrapper_data = WavpackGetWrapperData (wpc);
        int wrapper_bytes = WavpackGetWrapperBytes (wpc);

        // This is an odd case. Older versions of WavPack would store any data chunk padding bytes as
        // wrapper, but now we're generating them above based on the chunk size. To correctly handle
        // the former case, we'll eat an appropriate number of NULL wrapper bytes here.

        while (padding_bytes-- && wrapper_bytes && !*wrapper_data) {
            wrapper_bytes--;
            wrapper_data++;
        }

        if (!DoWriteFile (outfile, wrapper_data, wrapper_bytes, &bcount) || bcount != wrapper_bytes) {
            error_line ("can't write .WAV data, disk probably full!");
            DoTruncateFile (outfile);
            result = WAVPACK_HARD_ERROR;
        }

        WavpackFreeWrapper (wpc);
    }

    if (result == WAVPACK_NO_ERROR && outfile && created_riff_header &&
        (WavpackGetNumSamples64 (wpc) == -1 ||
         (until_samples_total ? until_samples_total : WavpackGetNumSamples64 (wpc)) != total_unpacked_samples)) {
            if (*outfilename == '-' || DoSetFilePositionAbsolute (outfile, 0))
                error_line ("can't update file header with actual size");
            else if (!file_formats [output_format].WriteHeader (outfile, wpc, total_unpacked_samples, output_qmode)) {
                DoTruncateFile (outfile);
                result = WAVPACK_HARD_ERROR;
            }
    }

    // if we are not just in verify only mode, flush the output stream and if it's a real file (not stdout)
    // close it and make sure it's not zero length (which means we got an error somewhere)

    if (outfile) {
        fflush (outfile);

        if (*outfilename != '-') {
            int64_t outfile_length = DoGetFileSize (outfile);

            if (!DoCloseHandle (outfile)) {
                error_line ("can't close file %s!", FN_FIT (outfilename));
                result = WAVPACK_SOFT_ERROR;
            }

            if (!outfile_length)
                DoDeleteFile (outfilename_temp ? outfilename_temp : outfilename);
        }
    }

    // if we were writing to a temp file because the target file already existed,
    // do the rename / overwrite now (and if that fails, flag the error)

#if defined(_WIN32)
    if (result == WAVPACK_NO_ERROR && outfilename && outfilename_temp) {
        if (remove (outfilename)) {
            error_line ("can not remove file %s, result saved in %s!", outfilename, outfilename_temp);
            result = WAVPACK_SOFT_ERROR;
        }
        else if (rename (outfilename_temp, outfilename)) {
            error_line ("can not rename temp file %s to %s!", outfilename_temp, outfilename);
            result = WAVPACK_SOFT_ERROR;
        }
    }
#else
    if (result == WAVPACK_NO_ERROR && outfilename && outfilename_temp && rename (outfilename_temp, outfilename)) {
        error_line ("can not rename temp file %s to %s!", outfilename_temp, outfilename);
        result = WAVPACK_SOFT_ERROR;
    }
#endif

    if (outfilename && outfilename_temp) free (outfilename_temp);

    if (result == WAVPACK_NO_ERROR && copy_time && outfilename &&
        !copy_timestamp (infilename, outfilename))
            error_line ("failure copying time stamp!");

    if (result == WAVPACK_NO_ERROR) {
        if (!until_samples_total && WavpackGetNumSamples64 (wpc) != -1) {
            if (total_unpacked_samples < WavpackGetNumSamples64 (wpc)) {
                error_line ("file is missing %llu samples!",
                    WavpackGetNumSamples64 (wpc) - total_unpacked_samples);
                result = WAVPACK_SOFT_ERROR;
            }
            else if (total_unpacked_samples > WavpackGetNumSamples64 (wpc)) {
                error_line ("file has %llu extra samples!",
                    total_unpacked_samples - WavpackGetNumSamples64 (wpc));
                result = WAVPACK_SOFT_ERROR;
            }
        }

        if (WavpackGetNumErrors (wpc)) {
            error_line ("missing data or crc errors detected in %d block(s)!", WavpackGetNumErrors (wpc));
            result = WAVPACK_SOFT_ERROR;
        }
    }

    if (result == WAVPACK_NO_ERROR && md5_diff && (WavpackGetMode (wpc) & MODE_LOSSLESS) && !until_samples_total && input_qmode == output_qmode) {
        error_line ("MD5 signatures should match, but do not!");
        result = WAVPACK_SOFT_ERROR;
    }

    // Compute and display the time consumed along with some other details of
    // the unpacking operation (assuming there was no error).

#if defined(_WIN32)
    _ftime64 (&time2);
    dtime = time2.time + time2.millitm / 1000.0;
    dtime -= time1.time + time1.millitm / 1000.0;
#else
    gettimeofday(&time2,&timez);
    dtime = time2.tv_sec + time2.tv_usec / 1000000.0;
    dtime -= time1.tv_sec + time1.tv_usec / 1000000.0;
#endif

    if (result == WAVPACK_NO_ERROR && !quiet_mode) {
        char *file, *fext, *oper, *cmode, cratio [16] = "";

        if (outfilename && *outfilename != '-') {
            file = FN_FIT (outfilename);
            fext = "";
            oper = "restored";
        }
        else {
            file = (*infilename == '-') ? "stdin" : FN_FIT (infilename);
            fext = wvc_mode ? " (+.wvc)" : "";
            oper = outfilename ? "unpacked" : "verified";
        }

        if (WavpackGetMode (wpc) & MODE_LOSSLESS) {
            cmode = "lossless";

            if (WavpackGetRatio (wpc) != 0.0)
                sprintf (cratio, ", %.2f%%", 100.0 - WavpackGetRatio (wpc) * 100.0);
        }
        else {
            cmode = "lossy";

            if (WavpackGetAverageBitrate (wpc, TRUE) != 0.0)
                sprintf (cratio, ", %d kbps", (int) (WavpackGetAverageBitrate (wpc, TRUE) / 1000.0));
        }

        error_line ("%s %s%s in %.2f secs (%s%s)", oper, file, fext, dtime, cmode, cratio);
    }

    WavpackCloseFile (wpc);

    if (result == WAVPACK_NO_ERROR && delete_source) {
        int res = DoDeleteFile (infilename);

        if (!quiet_mode || !res)
            error_line ("%s source file %s", res ?
                "deleted" : "can't delete", infilename);

        if (wvc_mode) {
            char in2filename [PATH_MAX];

            strcpy (in2filename, infilename);
            strcat (in2filename, "c");
            res = DoDeleteFile (in2filename);

            if (!quiet_mode || !res)
                error_line ("%s source file %s", res ?
                    "deleted" : "can't delete", in2filename);
        }
    }

    return result;
}