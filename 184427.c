static int quick_verify_file (char *infilename, int verbose)
{
    int64_t file_size, block_index, bytes_read = 0, total_samples = 0;
    int block_errors = 0, continuity_errors = 0, missing_checksums = 0, truncated = 0;
    int block_errors_c = 0, continuity_errors_c = 0, missing_checksums_c = 0, truncated_c = 0;
    int num_channels = 0, chan_index = 0, wvc_mode = 0, block_samples;
    FILE *(*fopen_func)(const char *, const char *) = fopen;
    double dtime, progress = -1.0;
    WavpackHeader wphdr, wphdr_c;
    unsigned char *block_buffer;
    FILE *infile, *infile_c;
    uint32_t bytes_skipped;

#ifdef _WIN32
    struct __timeb64 time1, time2;
#else
    struct timeval time1, time2;
    struct timezone timez;
#endif

#ifdef _WIN32
    fopen_func = fopen_utf8;
#endif

    if (*infilename == '-') {
        infile = stdin;
#ifdef _WIN32
        _setmode (fileno (stdin), O_BINARY);
#endif
#ifdef __OS2__
        setmode (fileno (stdin), O_BINARY);
#endif
    }
    else
        infile = fopen_func (infilename, "rb");

    if (!infile) {
        error_line ("quick verify: can't open file!");
        return WAVPACK_SOFT_ERROR;
    }

    file_size = DoGetFileSize (infile);
    bytes_skipped = read_next_header (infile, &wphdr);

    if (bytes_skipped == (uint32_t) -1) {
        fclose (infile);
        error_line ("quick verify: not a valid WavPack file!");
        return WAVPACK_SOFT_ERROR;
    }

    bytes_read = sizeof (wphdr) + bytes_skipped;

    // Legacy files without block checksums can't really be verified quickly. If they're a
    // a regular file we can retry with the regular verify, otherwise it's just a failure.

    if (!(wphdr.flags & HAS_CHECKSUM)) {
        if (*infilename == '-') {
            fclose (infile);
            error_line ("quick verify: legacy files cannot be quickly verified!");
            return WAVPACK_SOFT_ERROR;
        }
        else {
            fclose (infile);
            if (verbose) error_line ("quick verify: legacy file, switching to regular verify!");
            return WAVPACK_HARD_ERROR;
        }
    }

    // check for and open any correction file

    if ((wphdr.flags & HYBRID_FLAG) && infile != stdin && !ignore_wvc) {
        char *infilename_c = malloc (strlen (infilename) + 10);

        strcpy (infilename_c, infilename);
        strcat (infilename_c, "c");
        infile_c = fopen_func (infilename_c, "rb");
        free (infilename_c);

        if (infile_c) {
            wvc_mode = 1;
            file_size += DoGetFileSize (infile_c);
            bytes_skipped = read_next_header (infile_c, &wphdr_c);

            if (bytes_skipped == (uint32_t) -1) {
                fclose (infile); fclose (infile_c);
                error_line ("quick verify: not a valid WavPack correction file!");
                return WAVPACK_SOFT_ERROR;
            }

            bytes_read += sizeof (wphdr_c) + bytes_skipped;

            if (!(wphdr_c.flags & HAS_CHECKSUM)) {
                fclose (infile); fclose (infile_c);
                if (verbose) error_line ("quick verify: legacy correction file, switching to regular verify!");
                return WAVPACK_HARD_ERROR;
            }

            if (verbose) error_line ("quick verify: correction file found");
        }
    }

    if (!quiet_mode) {
        fprintf (stderr, "verifying %s%s,", *infilename == '-' ? "stdin" :
            FN_FIT (infilename), wvc_mode ? " (+.wvc)" : "");
        fflush (stderr);
    }

#if defined(_WIN32)
    _ftime64 (&time1);
#else
    gettimeofday(&time1,&timez);
#endif

    while (1) {

        // the continuity checks only apply to blocks with audio samples

        if (wphdr.block_samples) {

            // the first block with samples (indicated by total_samples == 0) has special significance

            if (!total_samples) {
                block_index = GET_BLOCK_INDEX (wphdr);

                if (block_index) {
                    if (verbose) error_line ("quick verify warning: file block index doesn't start at zero");
                    total_samples = -1;
                }
                else {
                    total_samples = GET_TOTAL_SAMPLES (wphdr);

                    if (total_samples == -1 && verbose)
                        error_line ("quick verify warning: file duration unknown");
                }
            }

            if (block_index != GET_BLOCK_INDEX (wphdr)) {
                block_index = GET_BLOCK_INDEX (wphdr);
                continuity_errors++;
            }

            if (wphdr.flags & INITIAL_BLOCK) {
                block_samples = wphdr.block_samples;
                chan_index = 0;
            }
            else if (wphdr.block_samples != block_samples)
                continuity_errors++;
        }

        // read the body of the block and use libwavpack to parse it and verify its checksum

        block_buffer = malloc (sizeof (wphdr) + wphdr.ckSize - 24);
        memcpy (block_buffer, &wphdr, sizeof (wphdr));

        if (!fread (block_buffer + sizeof (wphdr), wphdr.ckSize - 24, 1, infile)) {
            if (verbose) error_line ("quick verify error:%sfile is truncated!\n", wvc_mode ? " main " : " ");
            free (block_buffer);
            truncated = 1;
            break;
        }

        bytes_read += wphdr.ckSize - 24;

        // this is the libwavpack call that actually verifies the block's checksum

        if (!WavpackVerifySingleBlock (block_buffer, 1))
            block_errors++;

        free (block_buffer);

        // more stuff that only applies to blocks with audio...

        if (wphdr.block_samples) {

            // handle checking the corresponding correction file block here

            if (wvc_mode && !truncated_c) {
                unsigned char *block_buffer_c;
                int got_match = 0;

                while (!got_match && GET_BLOCK_INDEX (wphdr_c) <= GET_BLOCK_INDEX (wphdr)) {

                    if (GET_BLOCK_INDEX (wphdr_c) == GET_BLOCK_INDEX (wphdr)) {
                        if (wphdr_c.block_samples == wphdr.block_samples &&
                            wphdr_c.flags == wphdr.flags)
                                got_match = 1;
                            else
                                break;
                    }

                    block_buffer_c = malloc (sizeof (wphdr_c) + wphdr_c.ckSize - 24);
                    memcpy (block_buffer_c, &wphdr_c, sizeof (wphdr_c));

                    if (!fread (block_buffer_c + sizeof (wphdr_c), wphdr_c.ckSize - 24, 1, infile_c)) {
                        if (verbose) error_line ("quick verify error: correction file is truncated!");
                        free (block_buffer_c);
                        truncated_c = 1;
                        break;
                    }

                    bytes_read += wphdr_c.ckSize - 24;

                    if (!WavpackVerifySingleBlock (block_buffer_c, 1))
                        block_errors_c++;

                    bytes_skipped = read_next_header (infile_c, &wphdr_c);

                    if (bytes_skipped == (uint32_t) -1)
                        break;

                    bytes_read += sizeof (wphdr_c) + bytes_skipped;

                    if (!(wphdr_c.flags & HAS_CHECKSUM))
                        missing_checksums_c++;

                    if (bytes_skipped && verbose)
                        error_line ("quick verify warning: %u bytes skipped in correction file", bytes_skipped);
                }

                if (!got_match)
                    continuity_errors_c++;
            }

            chan_index += (wphdr.flags & MONO_FLAG) ? 1 : 2;

            // on the final block make sure we got all required channels, and get ready for the next sequence

            if (wphdr.flags & FINAL_BLOCK) {
                if (num_channels) {
                    if (num_channels != chan_index) {
                        if (verbose) error_line ("quick verify error: channel count changed %d --> %d", num_channels, chan_index);
                        num_channels = chan_index;
                        continuity_errors++;
                    }
                }
                else {
                    num_channels = chan_index;
                    if (verbose) error_line ("quick verify: channel count = %d", num_channels);
                }

                block_index += block_samples;
                chan_index = 0;
            }
        }

        // all done with that block, so read the next header

        bytes_skipped = read_next_header (infile, &wphdr);

        if (bytes_skipped == (uint32_t) -1)
            break;

        bytes_read += sizeof (wphdr) + bytes_skipped;

        // while all blocks should ideally have checksums, beta versions might not have
        // appended them to non-audio blocks, so we can ignore those cases for now

        if (wphdr.block_samples && !(wphdr.flags & HAS_CHECKSUM))
            missing_checksums++;

        if (bytes_skipped && verbose)
            error_line ("quick verify warning: %u bytes skipped", bytes_skipped);

        if (check_break ()) {
#if defined(_WIN32)
            fprintf (stderr, "^C\n");
#else
            fprintf (stderr, "\n");
#endif
            fflush (stderr);
            fclose (infile);
            if (wvc_mode) fclose (infile_c);
            return WAVPACK_SOFT_ERROR;
        }

        if (file_size && progress != floor ((double) bytes_read / file_size * 100.0 + 0.5)) {
            int nobs = progress == -1.0;

            progress = (double) bytes_read / file_size;
            display_progress (progress);
            progress = floor (progress * 100.0 + 0.5);

            if (!quiet_mode) {
                fprintf (stderr, "%s%3d%% done...",
                    nobs ? " " : "\b\b\b\b\b\b\b\b\b\b\b\b", (int) progress);
                fflush (stderr);
            }
        }
    }

    // all done, so close the files and report the results

    if (wvc_mode) fclose (infile_c);
    fclose (infile);

    if (truncated || block_errors || continuity_errors || missing_checksums ||
        truncated_c || block_errors_c || continuity_errors_c || missing_checksums_c) {
            int total_errors_c = truncated_c + block_errors_c + continuity_errors_c + missing_checksums_c;
            int total_errors = truncated + block_errors + continuity_errors + missing_checksums;

            if (verbose) {
                if (total_errors - truncated)
                    error_line ("quick verify%serrors: %d missing checksums, %d bad blocks, %d discontinuities!",
                        wvc_mode ? " [main] " : " ", missing_checksums, block_errors, continuity_errors);

                if (total_errors_c - truncated_c)
                    error_line ("quick verify [correction] errors: %d missing checksums, %d bad blocks, %d discontinuities!",
                        missing_checksums_c, block_errors_c, continuity_errors_c);
            }
            else {
                if (wvc_mode && !total_errors)
                    error_line ("quick verify: %d errors detected in correction file, main file okay!", total_errors_c);
                else if (wvc_mode)
                    error_line ("quick verify: %d errors detected in main and correction files!", total_errors + total_errors_c);
                else
                    error_line ("quick verify: %d errors detected!", total_errors);
            }

            return WAVPACK_SOFT_ERROR;
    }

    if (total_samples != -1 && total_samples != block_index) {
        if (total_samples < block_index)
            error_line ("quick verify: WavPack file contains %lld extra samples!", block_samples - total_samples);
        else
            error_line ("quick verify: WavPack file is missing %lld samples!", total_samples - block_samples);

        return WAVPACK_SOFT_ERROR;
    }

#if defined(_WIN32)
    _ftime64 (&time2);
    dtime = time2.time + time2.millitm / 1000.0;
    dtime -= time1.time + time1.millitm / 1000.0;
#else
    gettimeofday(&time2,&timez);
    dtime = time2.tv_sec + time2.tv_usec / 1000000.0;
    dtime -= time1.tv_sec + time1.tv_usec / 1000000.0;
#endif

    if (!quiet_mode) {
        char *file, *fext;

        file = (*infilename == '-') ? "stdin" : FN_FIT (infilename);
        fext = wvc_mode ? " (+.wvc)" : "";

        error_line ("quickly verified %s%s in %.2f secs", file, fext, dtime);
    }

    return WAVPACK_NO_ERROR;
}