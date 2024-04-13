static char *ga_get_current_arch(void)
{
    SYSTEM_INFO info;
    GetNativeSystemInfo(&info);
    char *result = NULL;
    switch (info.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_AMD64:
        result = g_strdup("x86_64");
        break;
    case PROCESSOR_ARCHITECTURE_ARM:
        result = g_strdup("arm");
        break;
    case PROCESSOR_ARCHITECTURE_IA64:
        result = g_strdup("ia64");
        break;
    case PROCESSOR_ARCHITECTURE_INTEL:
        result = g_strdup("x86");
        break;
    case PROCESSOR_ARCHITECTURE_UNKNOWN:
    default:
        slog("unknown processor architecture 0x%0x",
            info.wProcessorArchitecture);
        result = g_strdup("unknown");
        break;
    }
    return result;
}