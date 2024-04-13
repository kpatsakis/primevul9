void cpu_abort(CPUState *cpu, const char *fmt, ...)
{
    va_list ap;
    va_list ap2;

    va_start(ap, fmt);
    va_copy(ap2, ap);
    fprintf(stderr, "qemu: fatal: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    cpu_dump_state(cpu, stderr, fprintf, CPU_DUMP_FPU | CPU_DUMP_CCOP);
    if (qemu_log_enabled()) {
        qemu_log("qemu: fatal: ");
        qemu_log_vprintf(fmt, ap2);
        qemu_log("\n");
        log_cpu_state(cpu, CPU_DUMP_FPU | CPU_DUMP_CCOP);
        qemu_log_flush();
        qemu_log_close();
    }
    va_end(ap2);
    va_end(ap);
#if defined(CONFIG_USER_ONLY)
    {
        struct sigaction act;
        sigfillset(&act.sa_mask);
        act.sa_handler = SIG_DFL;
        sigaction(SIGABRT, &act, NULL);
    }
#endif
    abort();
}