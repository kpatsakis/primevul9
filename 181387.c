static V9fsFidState *clunk_fid(V9fsState *s, int32_t fid)
{
    V9fsFidState **fidpp, *fidp;

    for (fidpp = &s->fid_list; *fidpp; fidpp = &(*fidpp)->next) {
        if ((*fidpp)->fid == fid) {
            break;
        }
    }
    if (*fidpp == NULL) {
        return NULL;
    }
    fidp = *fidpp;
    *fidpp = fidp->next;
    fidp->clunked = 1;
    return fidp;
}