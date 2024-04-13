opj_bool pi_create_encode(opj_pi_iterator_t *pi, opj_cp_t *cp, int tileno,
                          int pino, int tpnum, int tppos, J2K_T2_MODE t2_mode, int cur_totnum_tp)
{
    char prog[4];
    int i;
    int incr_top = 1, resetX = 0;
    opj_tcp_t *tcps = &cp->tcps[tileno];
    opj_poc_t *tcp = &tcps->pocs[pino];

    pi[pino].first = 1;
    pi[pino].poc.prg = tcp->prg;

    switch (tcp->prg) {
    case CPRL:
        strncpy(prog, "CPRL", 4);
        break;
    case LRCP:
        strncpy(prog, "LRCP", 4);
        break;
    case PCRL:
        strncpy(prog, "PCRL", 4);
        break;
    case RLCP:
        strncpy(prog, "RLCP", 4);
        break;
    case RPCL:
        strncpy(prog, "RPCL", 4);
        break;
    case PROG_UNKNOWN:
        return OPJ_TRUE;
    }

    if (!(cp->tp_on && ((!cp->cinema && (t2_mode == FINAL_PASS)) || cp->cinema))) {
        pi[pino].poc.resno0 = tcp->resS;
        pi[pino].poc.resno1 = tcp->resE;
        pi[pino].poc.compno0 = tcp->compS;
        pi[pino].poc.compno1 = tcp->compE;
        pi[pino].poc.layno0 = tcp->layS;
        pi[pino].poc.layno1 = tcp->layE;
        pi[pino].poc.precno0 = tcp->prcS;
        pi[pino].poc.precno1 = tcp->prcE;
        pi[pino].poc.tx0 = tcp->txS;
        pi[pino].poc.ty0 = tcp->tyS;
        pi[pino].poc.tx1 = tcp->txE;
        pi[pino].poc.ty1 = tcp->tyE;
    } else {
        if (tpnum < cur_totnum_tp) {
            for (i = 3; i >= 0; i--) {
                switch (prog[i]) {
                case 'C':
                    if (i > tppos) {
                        pi[pino].poc.compno0 = tcp->compS;
                        pi[pino].poc.compno1 = tcp->compE;
                    } else {
                        if (tpnum == 0) {
                            tcp->comp_t = tcp->compS;
                            pi[pino].poc.compno0 = tcp->comp_t;
                            pi[pino].poc.compno1 = tcp->comp_t + 1;
                            tcp->comp_t += 1;
                        } else {
                            if (incr_top == 1) {
                                if (tcp->comp_t == tcp->compE) {
                                    tcp->comp_t = tcp->compS;
                                    pi[pino].poc.compno0 = tcp->comp_t;
                                    pi[pino].poc.compno1 = tcp->comp_t + 1;
                                    tcp->comp_t += 1;
                                    incr_top = 1;
                                } else {
                                    pi[pino].poc.compno0 = tcp->comp_t;
                                    pi[pino].poc.compno1 = tcp->comp_t + 1;
                                    tcp->comp_t += 1;
                                    incr_top = 0;
                                }
                            } else {
                                pi[pino].poc.compno0 = tcp->comp_t - 1;
                                pi[pino].poc.compno1 = tcp->comp_t;
                            }
                        }
                    }
                    break;

                case 'R':
                    if (i > tppos) {
                        pi[pino].poc.resno0 = tcp->resS;
                        pi[pino].poc.resno1 = tcp->resE;
                    } else {
                        if (tpnum == 0) {
                            tcp->res_t = tcp->resS;
                            pi[pino].poc.resno0 = tcp->res_t;
                            pi[pino].poc.resno1 = tcp->res_t + 1;
                            tcp->res_t += 1;
                        } else {
                            if (incr_top == 1) {
                                if (tcp->res_t == tcp->resE) {
                                    tcp->res_t = tcp->resS;
                                    pi[pino].poc.resno0 = tcp->res_t;
                                    pi[pino].poc.resno1 = tcp->res_t + 1;
                                    tcp->res_t += 1;
                                    incr_top = 1;
                                } else {
                                    pi[pino].poc.resno0 = tcp->res_t;
                                    pi[pino].poc.resno1 = tcp->res_t + 1;
                                    tcp->res_t += 1;
                                    incr_top = 0;
                                }
                            } else {
                                pi[pino].poc.resno0 = tcp->res_t - 1;
                                pi[pino].poc.resno1 = tcp->res_t;
                            }
                        }
                    }
                    break;

                case 'L':
                    if (i > tppos) {
                        pi[pino].poc.layno0 = tcp->layS;
                        pi[pino].poc.layno1 = tcp->layE;
                    } else {
                        if (tpnum == 0) {
                            tcp->lay_t = tcp->layS;
                            pi[pino].poc.layno0 = tcp->lay_t;
                            pi[pino].poc.layno1 = tcp->lay_t + 1;
                            tcp->lay_t += 1;
                        } else {
                            if (incr_top == 1) {
                                if (tcp->lay_t == tcp->layE) {
                                    tcp->lay_t = tcp->layS;
                                    pi[pino].poc.layno0 = tcp->lay_t;
                                    pi[pino].poc.layno1 = tcp->lay_t + 1;
                                    tcp->lay_t += 1;
                                    incr_top = 1;
                                } else {
                                    pi[pino].poc.layno0 = tcp->lay_t;
                                    pi[pino].poc.layno1 = tcp->lay_t + 1;
                                    tcp->lay_t += 1;
                                    incr_top = 0;
                                }
                            } else {
                                pi[pino].poc.layno0 = tcp->lay_t - 1;
                                pi[pino].poc.layno1 = tcp->lay_t;
                            }
                        }
                    }
                    break;

                case 'P':
                    switch (tcp->prg) {
                    case LRCP:
                    case RLCP:
                        if (i > tppos) {
                            pi[pino].poc.precno0 = tcp->prcS;
                            pi[pino].poc.precno1 = tcp->prcE;
                        } else {
                            if (tpnum == 0) {
                                tcp->prc_t = tcp->prcS;
                                pi[pino].poc.precno0 = tcp->prc_t;
                                pi[pino].poc.precno1 = tcp->prc_t + 1;
                                tcp->prc_t += 1;
                            } else {
                                if (incr_top == 1) {
                                    if (tcp->prc_t == tcp->prcE) {
                                        tcp->prc_t = tcp->prcS;
                                        pi[pino].poc.precno0 = tcp->prc_t;
                                        pi[pino].poc.precno1 = tcp->prc_t + 1;
                                        tcp->prc_t += 1;
                                        incr_top = 1;
                                    } else {
                                        pi[pino].poc.precno0 = tcp->prc_t;
                                        pi[pino].poc.precno1 = tcp->prc_t + 1;
                                        tcp->prc_t += 1;
                                        incr_top = 0;
                                    }
                                } else {
                                    pi[pino].poc.precno0 = tcp->prc_t - 1;
                                    pi[pino].poc.precno1 = tcp->prc_t;
                                }
                            }
                        }
                        break;
                    default:
                        if (i > tppos) {
                            pi[pino].poc.tx0 = tcp->txS;
                            pi[pino].poc.ty0 = tcp->tyS;
                            pi[pino].poc.tx1 = tcp->txE;
                            pi[pino].poc.ty1 = tcp->tyE;
                        } else {
                            if (tpnum == 0) {
                                tcp->tx0_t = tcp->txS;
                                tcp->ty0_t = tcp->tyS;
                                pi[pino].poc.tx0 = tcp->tx0_t;
                                pi[pino].poc.tx1 = tcp->tx0_t + tcp->dx - (tcp->tx0_t % tcp->dx);
                                pi[pino].poc.ty0 = tcp->ty0_t;
                                pi[pino].poc.ty1 = tcp->ty0_t + tcp->dy - (tcp->ty0_t % tcp->dy);
                                tcp->tx0_t = pi[pino].poc.tx1;
                                tcp->ty0_t = pi[pino].poc.ty1;
                            } else {
                                if (incr_top == 1) {
                                    if (tcp->tx0_t >= tcp->txE) {
                                        if (tcp->ty0_t >= tcp->tyE) {
                                            tcp->ty0_t = tcp->tyS;
                                            pi[pino].poc.ty0 = tcp->ty0_t;
                                            pi[pino].poc.ty1 = tcp->ty0_t + tcp->dy - (tcp->ty0_t % tcp->dy);
                                            tcp->ty0_t = pi[pino].poc.ty1;
                                            incr_top = 1;
                                            resetX = 1;
                                        } else {
                                            pi[pino].poc.ty0 = tcp->ty0_t;
                                            pi[pino].poc.ty1 = tcp->ty0_t + tcp->dy - (tcp->ty0_t % tcp->dy);
                                            tcp->ty0_t = pi[pino].poc.ty1;
                                            incr_top = 0;
                                            resetX = 1;
                                        }
                                        if (resetX == 1) {
                                            tcp->tx0_t = tcp->txS;
                                            pi[pino].poc.tx0 = tcp->tx0_t;
                                            pi[pino].poc.tx1 = tcp->tx0_t + tcp->dx - (tcp->tx0_t % tcp->dx);
                                            tcp->tx0_t = pi[pino].poc.tx1;
                                        }
                                    } else {
                                        pi[pino].poc.tx0 = tcp->tx0_t;
                                        pi[pino].poc.tx1 = tcp->tx0_t + tcp->dx - (tcp->tx0_t % tcp->dx);
                                        tcp->tx0_t = pi[pino].poc.tx1;
                                        pi[pino].poc.ty0 = tcp->ty0_t - tcp->dy - (tcp->ty0_t % tcp->dy);
                                        pi[pino].poc.ty1 = tcp->ty0_t ;
                                        incr_top = 0;
                                    }
                                } else {
                                    pi[pino].poc.tx0 = tcp->tx0_t - tcp->dx - (tcp->tx0_t % tcp->dx);
                                    pi[pino].poc.tx1 = tcp->tx0_t ;
                                    pi[pino].poc.ty0 = tcp->ty0_t - tcp->dy - (tcp->ty0_t % tcp->dy);
                                    pi[pino].poc.ty1 = tcp->ty0_t ;
                                }
                            }
                        }
                        break;
                    }
                    break;
                }
            }
        }
    }
    return OPJ_FALSE;
}