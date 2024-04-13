cmsBool smooth2(cmsContext ContextID, cmsFloat32Number w[], cmsFloat32Number y[], cmsFloat32Number z[], cmsFloat32Number lambda, int m)
{
    int i, i1, i2;
    cmsFloat32Number *c, *d, *e;
    cmsBool st;


    c = (cmsFloat32Number*) _cmsCalloc(ContextID, MAX_NODES_IN_CURVE, sizeof(cmsFloat32Number));
    d = (cmsFloat32Number*) _cmsCalloc(ContextID, MAX_NODES_IN_CURVE, sizeof(cmsFloat32Number));
    e = (cmsFloat32Number*) _cmsCalloc(ContextID, MAX_NODES_IN_CURVE, sizeof(cmsFloat32Number));

    if (c != NULL && d != NULL && e != NULL) {


    d[1] = w[1] + lambda;
    c[1] = -2 * lambda / d[1];
    e[1] = lambda /d[1];
    z[1] = w[1] * y[1];
    d[2] = w[2] + 5 * lambda - d[1] * c[1] *  c[1];
    c[2] = (-4 * lambda - d[1] * c[1] * e[1]) / d[2];
    e[2] = lambda / d[2];
    z[2] = w[2] * y[2] - c[1] * z[1];

    for (i = 3; i < m - 1; i++) {
        i1 = i - 1; i2 = i - 2;
        d[i]= w[i] + 6 * lambda - c[i1] * c[i1] * d[i1] - e[i2] * e[i2] * d[i2];
        c[i] = (-4 * lambda -d[i1] * c[i1] * e[i1])/ d[i];
        e[i] = lambda / d[i];
        z[i] = w[i] * y[i] - c[i1] * z[i1] - e[i2] * z[i2];
    }

    i1 = m - 2; i2 = m - 3;

    d[m - 1] = w[m - 1] + 5 * lambda -c[i1] * c[i1] * d[i1] - e[i2] * e[i2] * d[i2];
    c[m - 1] = (-2 * lambda - d[i1] * c[i1] * e[i1]) / d[m - 1];
    z[m - 1] = w[m - 1] * y[m - 1] - c[i1] * z[i1] - e[i2] * z[i2];
    i1 = m - 1; i2 = m - 2;

    d[m] = w[m] + lambda - c[i1] * c[i1] * d[i1] - e[i2] * e[i2] * d[i2];
    z[m] = (w[m] * y[m] - c[i1] * z[i1] - e[i2] * z[i2]) / d[m];
    z[m - 1] = z[m - 1] / d[m - 1] - c[m - 1] * z[m];

    for (i = m - 2; 1<= i; i--)
        z[i] = z[i] / d[i] - c[i] * z[i + 1] - e[i] * z[i + 2];

      st = TRUE;
    }
    else st = FALSE;

    if (c != NULL) _cmsFree(ContextID, c);
    if (d != NULL) _cmsFree(ContextID, d);
    if (e != NULL) _cmsFree(ContextID, e);

    return st;
}