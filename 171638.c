void pi_destroy(opj_pi_iterator_t *pi, opj_cp_t *cp, int tileno)
{
    int compno, pino;
    opj_tcp_t *tcp = &cp->tcps[tileno];
    if (pi) {
        for (pino = 0; pino < tcp->numpocs + 1; pino++) {
            if (pi[pino].comps) {
                for (compno = 0; compno < pi->numcomps; compno++) {
                    opj_pi_comp_t *comp = &pi[pino].comps[compno];
                    if (comp->resolutions) {
                        opj_free(comp->resolutions);
                    }
                }
                opj_free(pi[pino].comps);
            }
        }
        if (pi->include) {
            opj_free(pi->include);
        }
        opj_free(pi);
    }
}