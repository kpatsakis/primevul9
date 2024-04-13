static OpenFlags *find_open_flag(const char *mode_str)
{
    int mode;
    Error **errp = NULL;

    for (mode = 0; mode < ARRAY_SIZE(guest_file_open_modes); ++mode) {
        OpenFlags *flags = guest_file_open_modes + mode;

        if (strcmp(flags->forms, mode_str) == 0) {
            return flags;
        }
    }

    error_setg(errp, "invalid file open mode '%s'", mode_str);
    return NULL;
}