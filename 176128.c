static int opj_j2k_get_default_thread_count()
{
    const char* num_threads = getenv("OPJ_NUM_THREADS");
    if (num_threads == NULL || !opj_has_thread_support()) {
        return 0;
    }
    if (strcmp(num_threads, "ALL_CPUS") == 0) {
        return opj_get_num_cpus();
    }
    return atoi(num_threads);
}