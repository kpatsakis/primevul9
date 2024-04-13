
    static void _distance_scan(const unsigned int len,
                               const longT *const g,
                               longT (*const sep)(const longT, const longT, const longT *const),
                               longT (*const f)(const longT, const longT, const longT *const),
                               longT *const s,
                               longT *const t,
                               longT *const dt) {
      longT q = s[0] = t[0] = 0;
      for (int u = 1; u<(int)len; ++u) { // Forward scan.
        while ((q>=0) && f(t[q],s[q],g)>f(t[q],u,g)) { --q; }
        if (q<0) { q = 0; s[0] = u; }
        else { const longT w = 1 + sep(s[q], u, g); if (w<(longT)len) { ++q; s[q] = u; t[q] = w; }}
      }
      for (int u = (int)len - 1; u>=0; --u) { dt[u] = f(u,s[q],g); if (u==t[q]) --q; } // Backward scan.