vmxnet3_dump_conf_descr(const char *name,
                        struct Vmxnet3_VariableLenConfDesc *pm_descr)
{
    VMW_CFPRN("%s descriptor dump: Version %u, Length %u",
              name, pm_descr->confVer, pm_descr->confLen);

};