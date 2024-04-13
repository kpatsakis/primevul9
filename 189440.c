void jpc_init_t2state(jpc_enc_t *enc, int raflag)
{
/* It is assumed that band->numbps and cblk->numbps precomputed */

	jpc_enc_tcmpt_t *comp;
	jpc_enc_tcmpt_t *endcomps;
	jpc_enc_rlvl_t *lvl;
	jpc_enc_rlvl_t *endlvls;
	jpc_enc_band_t *band;
	jpc_enc_band_t *endbands;
	jpc_enc_cblk_t *cblk;
	jpc_enc_cblk_t *endcblks;
	jpc_enc_pass_t *pass;
	jpc_enc_pass_t *endpasses;
	jpc_tagtreenode_t *leaf;
	jpc_enc_tile_t *tile;
	int prcno;
	jpc_enc_prc_t *prc;

	tile = enc->curtile;

	endcomps = &tile->tcmpts[tile->numtcmpts];
	for (comp = tile->tcmpts; comp != endcomps; ++comp) {
		endlvls = &comp->rlvls[comp->numrlvls];
		for (lvl = comp->rlvls; lvl != endlvls; ++lvl) {
			if (!lvl->bands) {
				continue;
			}
			endbands = &lvl->bands[lvl->numbands];
			for (band = lvl->bands; band != endbands; ++band) {
				if (!band->data) {
					continue;
				}
				for (prcno = 0, prc = band->prcs; prcno < lvl->numprcs; ++prcno, ++prc) {
					if (!prc->cblks) {
						continue;
					}
					jpc_tagtree_reset(prc->incltree);
					jpc_tagtree_reset(prc->nlibtree);
					endcblks = &prc->cblks[prc->numcblks];
					for (cblk = prc->cblks; cblk != endcblks; ++cblk) {
						if (jas_stream_rewind(cblk->stream)) {
							assert(0);
						}
						cblk->curpass = (cblk->numpasses > 0) ? cblk->passes : 0;
						cblk->numencpasses = 0;
						cblk->numlenbits = 3;
						cblk->numimsbs = band->numbps - cblk->numbps;
						assert(cblk->numimsbs >= 0);
						leaf = jpc_tagtree_getleaf(prc->nlibtree, cblk - prc->cblks);
						jpc_tagtree_setvalue(prc->nlibtree, leaf, cblk->numimsbs);

						if (raflag) {
							endpasses = &cblk->passes[cblk->numpasses];
							for (pass = cblk->passes; pass != endpasses; ++pass) {
								pass->lyrno = -1;
								pass->lyrno = 0;
							}
						}
					}
				}
			}
		}
	}

}