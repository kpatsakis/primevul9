void jpc_save_t2state(jpc_enc_t *enc)
{
/* stream pos in embedded T1 stream may be wrong since not saved/restored! */

	jpc_enc_tcmpt_t *comp;
	jpc_enc_tcmpt_t *endcomps;
	jpc_enc_rlvl_t *lvl;
	jpc_enc_rlvl_t *endlvls;
	jpc_enc_band_t *band;
	jpc_enc_band_t *endbands;
	jpc_enc_cblk_t *cblk;
	jpc_enc_cblk_t *endcblks;
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
					jpc_tagtree_copy(prc->savincltree, prc->incltree);
					jpc_tagtree_copy(prc->savnlibtree, prc->nlibtree);
					endcblks = &prc->cblks[prc->numcblks];
					for (cblk = prc->cblks; cblk != endcblks; ++cblk) {
						cblk->savedcurpass = cblk->curpass;
						cblk->savednumencpasses = cblk->numencpasses;
						cblk->savednumlenbits = cblk->numlenbits;
					}
				}
			}
		}
	}

}