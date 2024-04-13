opj_bool pi_next(opj_pi_iterator_t * pi)
{
    switch (pi->poc.prg) {
    case LRCP:
        return pi_next_lrcp(pi);
    case RLCP:
        return pi_next_rlcp(pi);
    case RPCL:
        return pi_next_rpcl(pi);
    case PCRL:
        return pi_next_pcrl(pi);
    case CPRL:
        return pi_next_cprl(pi);
    case PROG_UNKNOWN:
        return OPJ_FALSE;
    }

    return OPJ_FALSE;
}