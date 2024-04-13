networkstatus_verify_bw_weights(networkstatus_t *ns)
{
  int64_t weight_scale;
  int64_t G=0, M=0, E=0, D=0, T=0;
  double Wgg, Wgm, Wgd, Wmg, Wmm, Wme, Wmd, Weg, Wem, Wee, Wed;
  double Gtotal=0, Mtotal=0, Etotal=0;
  const char *casename = NULL;
  int valid = 1;

  weight_scale = circuit_build_times_get_bw_scale(ns);
  Wgg = networkstatus_get_bw_weight(ns, "Wgg", -1);
  Wgm = networkstatus_get_bw_weight(ns, "Wgm", -1);
  Wgd = networkstatus_get_bw_weight(ns, "Wgd", -1);
  Wmg = networkstatus_get_bw_weight(ns, "Wmg", -1);
  Wmm = networkstatus_get_bw_weight(ns, "Wmm", -1);
  Wme = networkstatus_get_bw_weight(ns, "Wme", -1);
  Wmd = networkstatus_get_bw_weight(ns, "Wmd", -1);
  Weg = networkstatus_get_bw_weight(ns, "Weg", -1);
  Wem = networkstatus_get_bw_weight(ns, "Wem", -1);
  Wee = networkstatus_get_bw_weight(ns, "Wee", -1);
  Wed = networkstatus_get_bw_weight(ns, "Wed", -1);

  if (Wgg<0 || Wgm<0 || Wgd<0 || Wmg<0 || Wmm<0 || Wme<0 || Wmd<0 || Weg<0
          || Wem<0 || Wee<0 || Wed<0) {
    log_warn(LD_BUG, "No bandwidth weights produced in consensus!");
    return 0;
  }

  // First, sanity check basic summing properties that hold for all cases
  // We use > 1 as the check for these because they are computed as integers.
  // Sometimes there are rounding errors.
  if (fabs(Wmm - weight_scale) > 1) {
    log_warn(LD_BUG, "Wmm=%f != "I64_FORMAT,
             Wmm, I64_PRINTF_ARG(weight_scale));
    valid = 0;
  }

  if (fabs(Wem - Wee) > 1) {
    log_warn(LD_BUG, "Wem=%f != Wee=%f", Wem, Wee);
    valid = 0;
  }

  if (fabs(Wgm - Wgg) > 1) {
    log_warn(LD_BUG, "Wgm=%f != Wgg=%f", Wgm, Wgg);
    valid = 0;
  }

  if (fabs(Weg - Wed) > 1) {
    log_warn(LD_BUG, "Wed=%f != Weg=%f", Wed, Weg);
    valid = 0;
  }

  if (fabs(Wgg + Wmg - weight_scale) > 0.001*weight_scale) {
    log_warn(LD_BUG, "Wgg=%f != "I64_FORMAT" - Wmg=%f", Wgg,
             I64_PRINTF_ARG(weight_scale), Wmg);
    valid = 0;
  }

  if (fabs(Wee + Wme - weight_scale) > 0.001*weight_scale) {
    log_warn(LD_BUG, "Wee=%f != "I64_FORMAT" - Wme=%f", Wee,
             I64_PRINTF_ARG(weight_scale), Wme);
    valid = 0;
  }

  if (fabs(Wgd + Wmd + Wed - weight_scale) > 0.001*weight_scale) {
    log_warn(LD_BUG, "Wgd=%f + Wmd=%f + Wed=%f != "I64_FORMAT,
             Wgd, Wmd, Wed, I64_PRINTF_ARG(weight_scale));
    valid = 0;
  }

  Wgg /= weight_scale;
  Wgm /= weight_scale;
  Wgd /= weight_scale;

  Wmg /= weight_scale;
  Wmm /= weight_scale;
  Wme /= weight_scale;
  Wmd /= weight_scale;

  Weg /= weight_scale;
  Wem /= weight_scale;
  Wee /= weight_scale;
  Wed /= weight_scale;

  // Then, gather G, M, E, D, T to determine case
  SMARTLIST_FOREACH_BEGIN(ns->routerstatus_list, routerstatus_t *, rs) {
    if (rs->has_bandwidth) {
      T += rs->bandwidth;
      if (rs->is_exit && rs->is_possible_guard) {
        D += rs->bandwidth;
        Gtotal += Wgd*rs->bandwidth;
        Mtotal += Wmd*rs->bandwidth;
        Etotal += Wed*rs->bandwidth;
      } else if (rs->is_exit) {
        E += rs->bandwidth;
        Mtotal += Wme*rs->bandwidth;
        Etotal += Wee*rs->bandwidth;
      } else if (rs->is_possible_guard) {
        G += rs->bandwidth;
        Gtotal += Wgg*rs->bandwidth;
        Mtotal += Wmg*rs->bandwidth;
      } else {
        M += rs->bandwidth;
        Mtotal += Wmm*rs->bandwidth;
      }
    } else {
      log_warn(LD_BUG, "Missing consensus bandwidth for router %s",
               routerstatus_describe(rs));
    }
  } SMARTLIST_FOREACH_END(rs);

  // Finally, check equality conditions depending upon case 1, 2 or 3
  // Full equality cases: 1, 3b
  // Partial equality cases: 2b (E=G), 3a (M=E)
  // Fully unknown: 2a
  if (3*E >= T && 3*G >= T) {
    // Case 1: Neither are scarce
    casename = "Case 1";
    if (fabs(Etotal-Mtotal) > 0.01*MAX(Etotal,Mtotal)) {
      log_warn(LD_DIR,
               "Bw Weight Failure for %s: Etotal %f != Mtotal %f. "
               "G="I64_FORMAT" M="I64_FORMAT" E="I64_FORMAT" D="I64_FORMAT
               " T="I64_FORMAT". "
               "Wgg=%f Wgd=%f Wmg=%f Wme=%f Wmd=%f Wee=%f Wed=%f",
               casename, Etotal, Mtotal,
               I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
               I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
               Wgg, Wgd, Wmg, Wme, Wmd, Wee, Wed);
      valid = 0;
    }
    if (fabs(Etotal-Gtotal) > 0.01*MAX(Etotal,Gtotal)) {
      log_warn(LD_DIR,
               "Bw Weight Failure for %s: Etotal %f != Gtotal %f. "
               "G="I64_FORMAT" M="I64_FORMAT" E="I64_FORMAT" D="I64_FORMAT
               " T="I64_FORMAT". "
               "Wgg=%f Wgd=%f Wmg=%f Wme=%f Wmd=%f Wee=%f Wed=%f",
               casename, Etotal, Gtotal,
               I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
               I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
               Wgg, Wgd, Wmg, Wme, Wmd, Wee, Wed);
      valid = 0;
    }
    if (fabs(Gtotal-Mtotal) > 0.01*MAX(Gtotal,Mtotal)) {
      log_warn(LD_DIR,
               "Bw Weight Failure for %s: Mtotal %f != Gtotal %f. "
               "G="I64_FORMAT" M="I64_FORMAT" E="I64_FORMAT" D="I64_FORMAT
               " T="I64_FORMAT". "
               "Wgg=%f Wgd=%f Wmg=%f Wme=%f Wmd=%f Wee=%f Wed=%f",
               casename, Mtotal, Gtotal,
               I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
               I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
               Wgg, Wgd, Wmg, Wme, Wmd, Wee, Wed);
      valid = 0;
    }
  } else if (3*E < T && 3*G < T) {
    int64_t R = MIN(E, G);
    int64_t S = MAX(E, G);
    /*
     * Case 2: Both Guards and Exits are scarce
     * Balance D between E and G, depending upon
     * D capacity and scarcity. Devote no extra
     * bandwidth to middle nodes.
     */
    if (R+D < S) { // Subcase a
      double Rtotal, Stotal;
      if (E < G) {
        Rtotal = Etotal;
        Stotal = Gtotal;
      } else {
        Rtotal = Gtotal;
        Stotal = Etotal;
      }
      casename = "Case 2a";
      // Rtotal < Stotal
      if (Rtotal > Stotal) {
        log_warn(LD_DIR,
                   "Bw Weight Failure for %s: Rtotal %f > Stotal %f. "
                   "G="I64_FORMAT" M="I64_FORMAT" E="I64_FORMAT" D="I64_FORMAT
                   " T="I64_FORMAT". "
                   "Wgg=%f Wgd=%f Wmg=%f Wme=%f Wmd=%f Wee=%f Wed=%f",
                   casename, Rtotal, Stotal,
                   I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
                   I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
                   Wgg, Wgd, Wmg, Wme, Wmd, Wee, Wed);
        valid = 0;
      }
      // Rtotal < T/3
      if (3*Rtotal > T) {
        log_warn(LD_DIR,
                   "Bw Weight Failure for %s: 3*Rtotal %f > T "
                   I64_FORMAT". G="I64_FORMAT" M="I64_FORMAT" E="I64_FORMAT
                   " D="I64_FORMAT" T="I64_FORMAT". "
                   "Wgg=%f Wgd=%f Wmg=%f Wme=%f Wmd=%f Wee=%f Wed=%f",
                   casename, Rtotal*3, I64_PRINTF_ARG(T),
                   I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
                   I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
                   Wgg, Wgd, Wmg, Wme, Wmd, Wee, Wed);
        valid = 0;
      }
      // Stotal < T/3
      if (3*Stotal > T) {
        log_warn(LD_DIR,
                   "Bw Weight Failure for %s: 3*Stotal %f > T "
                   I64_FORMAT". G="I64_FORMAT" M="I64_FORMAT" E="I64_FORMAT
                   " D="I64_FORMAT" T="I64_FORMAT". "
                   "Wgg=%f Wgd=%f Wmg=%f Wme=%f Wmd=%f Wee=%f Wed=%f",
                   casename, Stotal*3, I64_PRINTF_ARG(T),
                   I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
                   I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
                   Wgg, Wgd, Wmg, Wme, Wmd, Wee, Wed);
        valid = 0;
      }
      // Mtotal > T/3
      if (3*Mtotal < T) {
        log_warn(LD_DIR,
                   "Bw Weight Failure for %s: 3*Mtotal %f < T "
                   I64_FORMAT". "
                   "G="I64_FORMAT" M="I64_FORMAT" E="I64_FORMAT" D="I64_FORMAT
                   " T="I64_FORMAT". "
                   "Wgg=%f Wgd=%f Wmg=%f Wme=%f Wmd=%f Wee=%f Wed=%f",
                   casename, Mtotal*3, I64_PRINTF_ARG(T),
                   I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
                   I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
                   Wgg, Wgd, Wmg, Wme, Wmd, Wee, Wed);
        valid = 0;
      }
    } else { // Subcase b: R+D > S
      casename = "Case 2b";

      /* Check the rare-M redirect case. */
      if (D != 0 && 3*M < T) {
        casename = "Case 2b (balanced)";
        if (fabs(Etotal-Mtotal) > 0.01*MAX(Etotal,Mtotal)) {
          log_warn(LD_DIR,
                   "Bw Weight Failure for %s: Etotal %f != Mtotal %f. "
                   "G="I64_FORMAT" M="I64_FORMAT" E="I64_FORMAT" D="I64_FORMAT
                   " T="I64_FORMAT". "
                   "Wgg=%f Wgd=%f Wmg=%f Wme=%f Wmd=%f Wee=%f Wed=%f",
                   casename, Etotal, Mtotal,
                   I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
                   I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
                   Wgg, Wgd, Wmg, Wme, Wmd, Wee, Wed);
          valid = 0;
        }
        if (fabs(Etotal-Gtotal) > 0.01*MAX(Etotal,Gtotal)) {
          log_warn(LD_DIR,
                   "Bw Weight Failure for %s: Etotal %f != Gtotal %f. "
                   "G="I64_FORMAT" M="I64_FORMAT" E="I64_FORMAT" D="I64_FORMAT
                   " T="I64_FORMAT". "
                   "Wgg=%f Wgd=%f Wmg=%f Wme=%f Wmd=%f Wee=%f Wed=%f",
                   casename, Etotal, Gtotal,
                   I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
                   I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
                   Wgg, Wgd, Wmg, Wme, Wmd, Wee, Wed);
          valid = 0;
        }
        if (fabs(Gtotal-Mtotal) > 0.01*MAX(Gtotal,Mtotal)) {
          log_warn(LD_DIR,
                   "Bw Weight Failure for %s: Mtotal %f != Gtotal %f. "
                   "G="I64_FORMAT" M="I64_FORMAT" E="I64_FORMAT" D="I64_FORMAT
                   " T="I64_FORMAT". "
                   "Wgg=%f Wgd=%f Wmg=%f Wme=%f Wmd=%f Wee=%f Wed=%f",
                   casename, Mtotal, Gtotal,
                   I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
                   I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
                   Wgg, Wgd, Wmg, Wme, Wmd, Wee, Wed);
          valid = 0;
        }
      } else {
        if (fabs(Etotal-Gtotal) > 0.01*MAX(Etotal,Gtotal)) {
          log_warn(LD_DIR,
                   "Bw Weight Failure for %s: Etotal %f != Gtotal %f. "
                   "G="I64_FORMAT" M="I64_FORMAT" E="I64_FORMAT" D="I64_FORMAT
                   " T="I64_FORMAT". "
                   "Wgg=%f Wgd=%f Wmg=%f Wme=%f Wmd=%f Wee=%f Wed=%f",
                   casename, Etotal, Gtotal,
                   I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
                   I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
                   Wgg, Wgd, Wmg, Wme, Wmd, Wee, Wed);
          valid = 0;
        }
      }
    }
  } else { // if (E < T/3 || G < T/3) {
    int64_t S = MIN(E, G);
    int64_t NS = MAX(E, G);
    if (3*(S+D) < T) { // Subcase a:
      double Stotal;
      double NStotal;
      if (G < E) {
        casename = "Case 3a (G scarce)";
        Stotal = Gtotal;
        NStotal = Etotal;
      } else { // if (G >= E) {
        casename = "Case 3a (E scarce)";
        NStotal = Gtotal;
        Stotal = Etotal;
      }
      // Stotal < T/3
      if (3*Stotal > T) {
        log_warn(LD_DIR,
                   "Bw Weight Failure for %s: 3*Stotal %f > T "
                   I64_FORMAT". G="I64_FORMAT" M="I64_FORMAT" E="I64_FORMAT
                   " D="I64_FORMAT" T="I64_FORMAT". "
                   "Wgg=%f Wgd=%f Wmg=%f Wme=%f Wmd=%f Wee=%f Wed=%f",
                   casename, Stotal*3, I64_PRINTF_ARG(T),
                   I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
                   I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
                   Wgg, Wgd, Wmg, Wme, Wmd, Wee, Wed);
        valid = 0;
      }
      if (NS >= M) {
        if (fabs(NStotal-Mtotal) > 0.01*MAX(NStotal,Mtotal)) {
          log_warn(LD_DIR,
                   "Bw Weight Failure for %s: NStotal %f != Mtotal %f. "
                   "G="I64_FORMAT" M="I64_FORMAT" E="I64_FORMAT" D="I64_FORMAT
                   " T="I64_FORMAT". "
                   "Wgg=%f Wgd=%f Wmg=%f Wme=%f Wmd=%f Wee=%f Wed=%f",
                   casename, NStotal, Mtotal,
                   I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
                   I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
                   Wgg, Wgd, Wmg, Wme, Wmd, Wee, Wed);
          valid = 0;
        }
      } else {
        // if NS < M, NStotal > T/3 because only one of G or E is scarce
        if (3*NStotal < T) {
          log_warn(LD_DIR,
                     "Bw Weight Failure for %s: 3*NStotal %f < T "
                     I64_FORMAT". G="I64_FORMAT" M="I64_FORMAT
                     " E="I64_FORMAT" D="I64_FORMAT" T="I64_FORMAT". "
                     "Wgg=%f Wgd=%f Wmg=%f Wme=%f Wmd=%f Wee=%f Wed=%f",
                     casename, NStotal*3, I64_PRINTF_ARG(T),
                     I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
                     I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
                     Wgg, Wgd, Wmg, Wme, Wmd, Wee, Wed);
          valid = 0;
        }
      }
    } else { // Subcase b: S+D >= T/3
      casename = "Case 3b";
      if (fabs(Etotal-Mtotal) > 0.01*MAX(Etotal,Mtotal)) {
        log_warn(LD_DIR,
                 "Bw Weight Failure for %s: Etotal %f != Mtotal %f. "
                 "G="I64_FORMAT" M="I64_FORMAT" E="I64_FORMAT" D="I64_FORMAT
                 " T="I64_FORMAT". "
                 "Wgg=%f Wgd=%f Wmg=%f Wme=%f Wmd=%f Wee=%f Wed=%f",
                 casename, Etotal, Mtotal,
                 I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
                 I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
                 Wgg, Wgd, Wmg, Wme, Wmd, Wee, Wed);
        valid = 0;
      }
      if (fabs(Etotal-Gtotal) > 0.01*MAX(Etotal,Gtotal)) {
        log_warn(LD_DIR,
                 "Bw Weight Failure for %s: Etotal %f != Gtotal %f. "
                 "G="I64_FORMAT" M="I64_FORMAT" E="I64_FORMAT" D="I64_FORMAT
                 " T="I64_FORMAT". "
                 "Wgg=%f Wgd=%f Wmg=%f Wme=%f Wmd=%f Wee=%f Wed=%f",
                 casename, Etotal, Gtotal,
                 I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
                 I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
                 Wgg, Wgd, Wmg, Wme, Wmd, Wee, Wed);
        valid = 0;
      }
      if (fabs(Gtotal-Mtotal) > 0.01*MAX(Gtotal,Mtotal)) {
        log_warn(LD_DIR,
                 "Bw Weight Failure for %s: Mtotal %f != Gtotal %f. "
                 "G="I64_FORMAT" M="I64_FORMAT" E="I64_FORMAT" D="I64_FORMAT
                 " T="I64_FORMAT". "
                 "Wgg=%f Wgd=%f Wmg=%f Wme=%f Wmd=%f Wee=%f Wed=%f",
                 casename, Mtotal, Gtotal,
                 I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
                 I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
                 Wgg, Wgd, Wmg, Wme, Wmd, Wee, Wed);
        valid = 0;
      }
    }
  }

  if (valid)
    log_notice(LD_DIR, "Bandwidth-weight %s is verified and valid.",
               casename);

  return valid;
}