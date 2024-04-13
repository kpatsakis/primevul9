matrix_is_compatible(const gs_matrix *pmat1, const gs_matrix *pmat2)
{
    return (pmat2->xx == pmat1->xx && pmat2->xy == pmat1->xy &&
            pmat2->yx == pmat1->yx && pmat2->yy == pmat1->yy);
}