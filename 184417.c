void display_progress (double file_progress)
{
    char title [40];

    if (set_console_title) {
        file_progress = (file_index + file_progress) / num_files;
        sprintf (title, "%d%% (WvUnpack)", (int) ((file_progress * 100.0) + 0.5));
        DoSetConsoleTitle (title);
    }
}