static int test_configuration(bool setting_SaveFullCore, bool setting_CreateCoreBacktrace)
{
    if (!setting_SaveFullCore && !setting_CreateCoreBacktrace)
    {
        fprintf(stderr, "Both SaveFullCore and CreateCoreBacktrace are disabled - "
                        "at least one of them is needed for useful report.\n");
        return 1;
    }

    return 0;
}