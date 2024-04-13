static void test_map_big_memory(void)
{
    uc_engine *uc;

    OK(uc_open(UC_ARCH_X86, UC_MODE_64, &uc));

    uc_assert_err(UC_ERR_NOMEM,
                  uc_mem_map(uc, 0x0, 0xfffffffffffff000, UC_PROT_ALL));

    OK(uc_close(uc));
}