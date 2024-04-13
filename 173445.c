unsigned PackLinuxElf32::find_LOAD_gap(
    Elf32_Phdr const *const phdr,
    unsigned const k,
    unsigned const nph
)
{
    if (!is_LOAD32(&phdr[k])) {
        return 0;
    }
    unsigned const hi = get_te32(&phdr[k].p_offset) +
                        get_te32(&phdr[k].p_filesz);
    unsigned lo = ph.u_file_size;
    if (lo < hi)
        throwCantPack("bad input: PT_LOAD beyond end-of-file");
    unsigned j = k;
    for (;;) { // circular search, optimize for adjacent ascending
        ++j;
        if (nph==j) {
            j = 0;
        }
        if (k==j) {
            break;
        }
        if (is_LOAD32(&phdr[j])) {
            unsigned const t = get_te32(&phdr[j].p_offset);
            if ((t - hi) < (lo - hi)) {
                lo = t;
                if (hi==lo) {
                    break;
                }
            }
        }
    }
    return lo - hi;
}