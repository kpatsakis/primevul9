find_open_flag(const char *mode_str, Error **errp)
{
    unsigned mode;

    for (mode = 0; mode < ARRAY_SIZE(guest_file_open_modes); ++mode) {
        ccpc *form;

        form = guest_file_open_modes[mode].forms;
        while (*form != NULL && strcmp(*form, mode_str) != 0) {
            ++form;
        }
        if (*form != NULL) {
            break;
        }
    }

    if (mode == ARRAY_SIZE(guest_file_open_modes)) {
        error_setg(errp, "invalid file open mode '%s'", mode_str);
        return -1;
    }
    return guest_file_open_modes[mode].oflag_base | O_NOCTTY | O_NONBLOCK;
}