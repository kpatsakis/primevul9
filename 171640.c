opj_pi_iterator_t *pi_create_decode(opj_image_t *image, opj_cp_t *cp,
                                    int tileno)
{
    int p, q;
    int compno, resno, pino;
    opj_pi_iterator_t *pi = NULL;
    opj_tcp_t *tcp = NULL;
    opj_tccp_t *tccp = NULL;

    tcp = &cp->tcps[tileno];

    pi = (opj_pi_iterator_t*) opj_calloc((tcp->numpocs + 1),
                                         sizeof(opj_pi_iterator_t));
    if (!pi) {
        /* TODO: throw an error */
        return NULL;
    }

    for (pino = 0; pino < tcp->numpocs + 1; pino++) {   /* change */
        int maxres = 0;
        int maxprec = 0;
        p = tileno % cp->tw;
        q = tileno / cp->tw;

        pi[pino].tx0 = int_max(cp->tx0 + p * cp->tdx, image->x0);
        pi[pino].ty0 = int_max(cp->ty0 + q * cp->tdy, image->y0);
        pi[pino].tx1 = int_min(cp->tx0 + (p + 1) * cp->tdx, image->x1);
        pi[pino].ty1 = int_min(cp->ty0 + (q + 1) * cp->tdy, image->y1);
        pi[pino].numcomps = image->numcomps;

        pi[pino].comps = (opj_pi_comp_t*) opj_calloc(image->numcomps,
                         sizeof(opj_pi_comp_t));
        if (!pi[pino].comps) {
            /* TODO: throw an error */
            pi_destroy(pi, cp, tileno);
            return NULL;
        }

        for (compno = 0; compno < pi->numcomps; compno++) {
            int tcx0, tcy0, tcx1, tcy1;
            opj_pi_comp_t *comp = &pi[pino].comps[compno];
            tccp = &tcp->tccps[compno];
            comp->dx = image->comps[compno].dx;
            comp->dy = image->comps[compno].dy;
            comp->numresolutions = tccp->numresolutions;

            comp->resolutions = (opj_pi_resolution_t*) opj_calloc(comp->numresolutions,
                                sizeof(opj_pi_resolution_t));
            if (!comp->resolutions) {
                /* TODO: throw an error */
                pi_destroy(pi, cp, tileno);
                return NULL;
            }

            tcx0 = int_ceildiv(pi->tx0, comp->dx);
            tcy0 = int_ceildiv(pi->ty0, comp->dy);
            tcx1 = int_ceildiv(pi->tx1, comp->dx);
            tcy1 = int_ceildiv(pi->ty1, comp->dy);
            if (comp->numresolutions > maxres) {
                maxres = comp->numresolutions;
            }

            for (resno = 0; resno < comp->numresolutions; resno++) {
                int levelno;
                int rx0, ry0, rx1, ry1;
                int px0, py0, px1, py1;
                opj_pi_resolution_t *res = &comp->resolutions[resno];
                if (tccp->csty & J2K_CCP_CSTY_PRT) {
                    res->pdx = tccp->prcw[resno];
                    res->pdy = tccp->prch[resno];
                } else {
                    res->pdx = 15;
                    res->pdy = 15;
                }
                levelno = comp->numresolutions - 1 - resno;
                rx0 = int_ceildivpow2(tcx0, levelno);
                ry0 = int_ceildivpow2(tcy0, levelno);
                rx1 = int_ceildivpow2(tcx1, levelno);
                ry1 = int_ceildivpow2(tcy1, levelno);
                px0 = int_floordivpow2(rx0, res->pdx) << res->pdx;
                py0 = int_floordivpow2(ry0, res->pdy) << res->pdy;
                px1 = int_ceildivpow2(rx1, res->pdx) << res->pdx;
                py1 = int_ceildivpow2(ry1, res->pdy) << res->pdy;
                res->pw = (rx0 == rx1) ? 0 : ((px1 - px0) >> res->pdx);
                res->ph = (ry0 == ry1) ? 0 : ((py1 - py0) >> res->pdy);

                if (res->pw * res->ph > maxprec) {
                    maxprec = res->pw * res->ph;
                }

            }
        }

        tccp = &tcp->tccps[0];
        pi[pino].step_p = 1;
        pi[pino].step_c = maxprec * pi[pino].step_p;
        pi[pino].step_r = image->numcomps * pi[pino].step_c;
        pi[pino].step_l = maxres * pi[pino].step_r;

        if (pino == 0) {
            pi[pino].include = (short int*) opj_calloc(image->numcomps * maxres *
                               tcp->numlayers * maxprec, sizeof(short int));
            if (!pi[pino].include) {
                /* TODO: throw an error */
                pi_destroy(pi, cp, tileno);
                return NULL;
            }
        } else {
            pi[pino].include = pi[pino - 1].include;
        }

        if (tcp->POC == 0) {
            pi[pino].first = 1;
            pi[pino].poc.resno0 = 0;
            pi[pino].poc.compno0 = 0;
            pi[pino].poc.layno1 = tcp->numlayers;
            pi[pino].poc.resno1 = maxres;
            pi[pino].poc.compno1 = image->numcomps;
            pi[pino].poc.prg = tcp->prg;
        } else {
            pi[pino].first = 1;
            pi[pino].poc.resno0 = tcp->pocs[pino].resno0;
            pi[pino].poc.compno0 = tcp->pocs[pino].compno0;
            pi[pino].poc.layno1 = tcp->pocs[pino].layno1;
            pi[pino].poc.resno1 = tcp->pocs[pino].resno1;
            pi[pino].poc.compno1 = tcp->pocs[pino].compno1;
            pi[pino].poc.prg = tcp->pocs[pino].prg;
        }
        pi[pino].poc.layno0  = 0;
        pi[pino].poc.precno0 = 0;
        pi[pino].poc.precno1 = maxprec;

    }

    return pi;
}