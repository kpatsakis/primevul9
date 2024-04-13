static int do_tag_extractions (WavpackContext *wpc, char *outfilename)
{
    int result = WAVPACK_NO_ERROR, i;
    FILE *outfile;

    for (i = 0; result == WAVPACK_NO_ERROR && i < num_tag_extractions; ++i) {
        char *extraction_spec = strdup (tag_extractions [i]);
        char *output_spec = strchr (extraction_spec, '=');
        char tag_filename [256];

        if (output_spec && output_spec > extraction_spec && strlen (output_spec) > 1)
            *output_spec++ = 0;

        if (dump_tag_item_to_file (wpc, extraction_spec, NULL, tag_filename)) {
            int max_length = (int) strlen (outfilename) + (int) strlen (tag_filename) + 10;
            char *full_filename;

            if (output_spec)
                max_length += (int) strlen (output_spec) + 256;

            full_filename = malloc (max_length * 2 + 1);
            strcpy (full_filename, outfilename);

            if (output_spec) {
                char *dst = filespec_name (full_filename);

                while (*output_spec && dst - full_filename < max_length) {
                    if (*output_spec == '%') {
                        switch (*++output_spec) {
                            case 'a':                           // audio filename
                                strcpy (dst, filespec_name (outfilename));

                                if (filespec_ext (dst))         // get rid of any extension
                                    dst = filespec_ext (dst);
                                else
                                    dst += strlen (dst);

                                output_spec++;
                                break;

                            case 't':                           // tag field name
                                strcpy (dst, tag_filename);

                                if (filespec_ext (dst))         // get rid of any extension
                                    dst = filespec_ext (dst);
                                else
                                    dst += strlen (dst);

                                output_spec++;
                                break;

                            case 'e':                           // default extension
                                if (filespec_ext (tag_filename)) {
                                    strcpy (dst, filespec_ext (tag_filename) + 1);
                                    dst += strlen (dst);
                                }

                                output_spec++;
                                break;

                            default:
                                *dst++ = '%';
                        }
                    }
                    else
                        *dst++ = *output_spec++;
                }

                *dst = 0;
            }
            else
                strcpy (filespec_name (full_filename), tag_filename);

            if (!overwrite_all && (outfile = fopen (full_filename, "r")) != NULL) {
                DoCloseHandle (outfile);
                fprintf (stderr, "overwrite %s (yes/no/all)? ", FN_FIT (full_filename));
                fflush (stderr);

                if (set_console_title)
                    DoSetConsoleTitle ("overwrite?");

                switch (yna ()) {

                    case 'n':
                        *full_filename = 0;
                        break;

                    case 'a':
                        overwrite_all = 1;
                }
            }

            // open output file for writing

            if (*full_filename) {
                if ((outfile = fopen (full_filename, "w")) == NULL) {
                    error_line ("can't create file %s!", FN_FIT (full_filename));
                    result = WAVPACK_SOFT_ERROR;
                }
                else {
                    dump_tag_item_to_file (wpc, extraction_spec, outfile, NULL);

                    if (!DoCloseHandle (outfile)) {
                        error_line ("can't close file %s!", FN_FIT (full_filename));
                        result = WAVPACK_SOFT_ERROR;
                    }
                    else if (!quiet_mode)
                        error_line ("extracted tag \"%s\" to file %s", extraction_spec, FN_FIT (full_filename));
                }
            }

            free (full_filename);
        }

        free (extraction_spec);
    }

    return result;
}