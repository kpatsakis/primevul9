static FILE *open_output_file (char *filename, char **tempfilename)
{
    FILE *retval, *testfile;
    char dummy;

    *tempfilename = NULL;

    if (*filename == '-') {
#if defined(_WIN32)
        _setmode (_fileno (stdout), O_BINARY);
#endif
#if defined(__OS2__)
        setmode (fileno (stdout), O_BINARY);
#endif
        return stdout;
    }

    testfile = fopen (filename, "rb");

    if (testfile) {
        size_t res = fread (&dummy, 1, 1, testfile);

        fclose (testfile);

        if (res == 1) {
            int count = 0;

            if (!overwrite_all) {
                fprintf (stderr, "overwrite %s (yes/no/all)? ", FN_FIT (filename));
                fflush (stderr);

                if (set_console_title)
                    DoSetConsoleTitle ("overwrite?");

                switch (yna ()) {
                    case 'n':
                        return NULL;

                    case 'a':
                        overwrite_all = 1;
                }
            }

            *tempfilename = malloc (strlen (filename) + 16);

            while (1) {
                strcpy (*tempfilename, filename);

                if (filespec_ext (*tempfilename)) {
                    if (count++)
                        sprintf (filespec_ext (*tempfilename), ".tmp%d", count-1);
                    else
                        strcpy (filespec_ext (*tempfilename), ".tmp");

                    strcat (*tempfilename, filespec_ext (filename));
                }
                else {
                    if (count++)
                        sprintf (*tempfilename + strlen (*tempfilename), ".tmp%d", count-1);
                    else
                        strcat (*tempfilename, ".tmp");
                }

                testfile = fopen (*tempfilename, "rb");

                if (!testfile)
                    break;

                res = fread (&dummy, 1, 1, testfile);
                fclose (testfile);

                if (res != 1)
                    break;
            }
        }
    }

    retval = fopen (*tempfilename ? *tempfilename : filename, "w+b");

    if (retval == NULL)
        error_line ("can't create file %s!", *tempfilename ? *tempfilename : filename);

    return retval;
}