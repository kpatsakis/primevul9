static int jpc_enc_encodemainbody(jpc_enc_t *enc)
{
	int tileno;
	int tilex;
	int tiley;
	int i;
	jpc_sot_t *sot;
	jpc_enc_tcmpt_t *comp;
	jpc_enc_tcmpt_t *endcomps;
	jpc_enc_band_t *band;
	jpc_enc_band_t *endbands;
	jpc_enc_rlvl_t *lvl;
	int rlvlno;
	jpc_qcc_t *qcc;
	jpc_cod_t *cod;
	int adjust;
	int j;
	int absbandno;
	long numbytes;
	long tilehdrlen;
	long tilelen;
	jpc_enc_tile_t *tile;
	jpc_enc_cp_t *cp;
	double rho;
	int lyrno;
	int cmptno;
	int samestepsizes;
	jpc_enc_ccp_t *ccps;
	jpc_enc_tccp_t *tccp;
	int bandno;
	uint_fast32_t x;
	uint_fast32_t y;
	int mingbits;
	int actualnumbps;
	jpc_fix_t mxmag;
	jpc_fix_t mag;
	int numgbits;

	cp = enc->cp;

	/* Avoid compile warnings. */
	numbytes = 0;

	for (tileno = 0; tileno < JAS_CAST(int, cp->numtiles); ++tileno) {
		tilex = tileno % cp->numhtiles;
		tiley = tileno / cp->numhtiles;

		if (!(enc->curtile = jpc_enc_tile_create(enc->cp, enc->image,
		  tileno))) {
			jas_eprintf("cannot create tile\n");
			return -1;
		}

		tile = enc->curtile;

		if (jas_getdbglevel() >= 10) {
			jpc_enc_dump(enc);
		}

		endcomps = &tile->tcmpts[tile->numtcmpts];
		for (cmptno = 0, comp = tile->tcmpts; cmptno < tile->numtcmpts; ++cmptno, ++comp) {
			if (!cp->ccps[cmptno].sgnd) {
				adjust = 1 << (cp->ccps[cmptno].prec - 1);
				for (i = 0; i < jas_matrix_numrows(comp->data); ++i) {
					for (j = 0; j < jas_matrix_numcols(comp->data); ++j) {
						*jas_matrix_getref(comp->data, i, j) -= adjust;
					}
				}
			}
		}

		if (!tile->intmode) {
				endcomps = &tile->tcmpts[tile->numtcmpts];
				for (comp = tile->tcmpts; comp != endcomps; ++comp) {
					jas_matrix_asl(comp->data, JPC_FIX_FRACBITS);
				}
		}

		switch (tile->mctid) {
		case JPC_MCT_RCT:
assert(jas_image_numcmpts(enc->image) == 3);
			jpc_rct(tile->tcmpts[0].data, tile->tcmpts[1].data,
			  tile->tcmpts[2].data);
			break;
		case JPC_MCT_ICT:
assert(jas_image_numcmpts(enc->image) == 3);
			jpc_ict(tile->tcmpts[0].data, tile->tcmpts[1].data,
			  tile->tcmpts[2].data);
			break;
		default:
			break;
		}

		for (i = 0; i < jas_image_numcmpts(enc->image); ++i) {
			comp = &tile->tcmpts[i];
			jpc_tsfb_analyze(comp->tsfb, comp->data);

		}


		endcomps = &tile->tcmpts[tile->numtcmpts];
		for (cmptno = 0, comp = tile->tcmpts; comp != endcomps; ++cmptno, ++comp) {
			mingbits = 0;
			absbandno = 0;
			/* All bands must have a corresponding quantizer step size,
			  even if they contain no samples and are never coded. */
			/* Some bands may not be hit by the loop below, so we must
			  initialize all of the step sizes to a sane value. */
			memset(comp->stepsizes, 0, sizeof(comp->stepsizes));
			for (rlvlno = 0, lvl = comp->rlvls; rlvlno < comp->numrlvls; ++rlvlno, ++lvl) {
				if (!lvl->bands) {
					absbandno += rlvlno ? 3 : 1;
					continue;
				}
				endbands = &lvl->bands[lvl->numbands];
				for (band = lvl->bands; band != endbands; ++band) {
					if (!band->data) {
						++absbandno;
						continue;
					}
					actualnumbps = 0;
					mxmag = 0;
					for (y = 0; y < JAS_CAST(uint_fast32_t, jas_matrix_numrows(band->data)); ++y) {
						for (x = 0; x < JAS_CAST(uint_fast32_t, jas_matrix_numcols(band->data)); ++x) {
							mag = JAS_ABS(jas_matrix_get(band->data, y, x));
							if (mag > mxmag) {
								mxmag = mag;
							}
						}
					}
					if (tile->intmode) {
						actualnumbps = jpc_fix_firstone(mxmag) + 1;
					} else {
						actualnumbps = jpc_fix_firstone(mxmag) + 1 - JPC_FIX_FRACBITS;
					}
					numgbits = actualnumbps - (cp->ccps[cmptno].prec - 1 +
					  band->analgain);
#if 0
jas_eprintf("%d %d mag=%d actual=%d numgbits=%d\n", cp->ccps[cmptno].prec, band->analgain, mxmag, actualnumbps, numgbits);
#endif
					if (numgbits > mingbits) {
						mingbits = numgbits;
					}
					if (!tile->intmode) {
						band->absstepsize = jpc_fix_div(jpc_inttofix(1
						  << (band->analgain + 1)),
						  band->synweight);
					} else {
						band->absstepsize = jpc_inttofix(1);
					}
					const uint_fast32_t stepsize = jpc_abstorelstepsize(
					  band->absstepsize, cp->ccps[cmptno].prec +
					  band->analgain);
					if (stepsize == UINT_FAST32_MAX)
						return -1;
					band->stepsize = stepsize;
					band->numbps = cp->tccp.numgbits +
					  JPC_QCX_GETEXPN(band->stepsize) - 1;

					if ((!tile->intmode) && band->data) {
						jpc_quantize(band->data, band->absstepsize);
					}

					comp->stepsizes[absbandno] = band->stepsize;
					++absbandno;
				}
			}

			assert(JPC_FIX_FRACBITS >= JPC_NUMEXTRABITS);
			if (!tile->intmode) {
				jas_matrix_divpow2(comp->data, JPC_FIX_FRACBITS - JPC_NUMEXTRABITS);
			} else {
				jas_matrix_asl(comp->data, JPC_NUMEXTRABITS);
			}

#if 0
jas_eprintf("mingbits %d\n", mingbits);
#endif
			if (mingbits > cp->tccp.numgbits) {
				jas_eprintf("error: too few guard bits (need at least %d)\n",
				  mingbits);
				return -1;
			}
		}

		if (!(enc->tmpstream = jas_stream_memopen(0, 0))) {
			jas_eprintf("cannot open tmp file\n");
			return -1;
		}

		/* Write the tile header. */
		if (!(enc->mrk = jpc_ms_create(JPC_MS_SOT))) {
			return -1;
		}
		sot = &enc->mrk->parms.sot;
		sot->len = 0;
		sot->tileno = tileno;
		sot->partno = 0;
		sot->numparts = 1;
		if (jpc_putms(enc->tmpstream, enc->cstate, enc->mrk)) {
			jas_eprintf("cannot write SOT marker\n");
			return -1;
		}
		jpc_ms_destroy(enc->mrk);
		enc->mrk = 0;

/************************************************************************/
/************************************************************************/
/************************************************************************/

		tccp = &cp->tccp;
		for (cmptno = 0; cmptno < JAS_CAST(int, cp->numcmpts); ++cmptno) {
			comp = &tile->tcmpts[cmptno];
			if (comp->numrlvls != tccp->maxrlvls) {
				if (!(enc->mrk = jpc_ms_create(JPC_MS_COD))) {
					return -1;
				}
/* XXX = this is not really correct. we are using comp #0's precint sizes
and other characteristics */
				comp = &tile->tcmpts[0];
				cod = &enc->mrk->parms.cod;
				cod->compparms.csty = 0;
				cod->compparms.numdlvls = comp->numrlvls - 1;
				cod->prg = tile->prg;
				cod->numlyrs = tile->numlyrs;
				cod->compparms.cblkwidthval = JPC_COX_CBLKSIZEEXPN(comp->cblkwidthexpn);
				cod->compparms.cblkheightval = JPC_COX_CBLKSIZEEXPN(comp->cblkheightexpn);
				cod->compparms.cblksty = comp->cblksty;
				cod->compparms.qmfbid = comp->qmfbid;
				cod->mctrans = (tile->mctid != JPC_MCT_NONE);
				for (i = 0; i < comp->numrlvls; ++i) {
					cod->compparms.rlvls[i].parwidthval = comp->rlvls[i].prcwidthexpn;
					cod->compparms.rlvls[i].parheightval = comp->rlvls[i].prcheightexpn;
				}
				if (jpc_putms(enc->tmpstream, enc->cstate, enc->mrk)) {
					return -1;
				}
				jpc_ms_destroy(enc->mrk);
				enc->mrk = 0;
			}
		}

		for (cmptno = 0, comp = tile->tcmpts; cmptno < JAS_CAST(int,
		  cp->numcmpts); ++cmptno, ++comp) {
			ccps = &cp->ccps[cmptno];
			if (JAS_CAST(int, ccps->numstepsizes) == comp->numstepsizes) {
				samestepsizes = 1;
				for (bandno = 0; bandno < JAS_CAST(int, ccps->numstepsizes);
				  ++bandno) {
					if (ccps->stepsizes[bandno] != comp->stepsizes[bandno]) {
						samestepsizes = 0;
						break;
					}
				}
			} else {
				samestepsizes = 0;
			}
			if (!samestepsizes) {
				if (!(enc->mrk = jpc_ms_create(JPC_MS_QCC))) {
					return -1;
				}
				qcc = &enc->mrk->parms.qcc;
				qcc->compno = cmptno;
				qcc->compparms.numguard = cp->tccp.numgbits;
				qcc->compparms.qntsty = (comp->qmfbid == JPC_COX_INS) ?
				  JPC_QCX_SEQNT : JPC_QCX_NOQNT;
				qcc->compparms.numstepsizes = comp->numstepsizes;
				qcc->compparms.stepsizes = comp->stepsizes;
				if (jpc_putms(enc->tmpstream, enc->cstate, enc->mrk)) {
					return -1;
				}
				qcc->compparms.stepsizes = 0;
				jpc_ms_destroy(enc->mrk);
				enc->mrk = 0;
			}
		}

		/* Write a SOD marker to indicate the end of the tile header. */
		if (!(enc->mrk = jpc_ms_create(JPC_MS_SOD))) {
			return -1;
		}
		if (jpc_putms(enc->tmpstream, enc->cstate, enc->mrk)) {
			jas_eprintf("cannot write SOD marker\n");
			return -1;
		}
		jpc_ms_destroy(enc->mrk);
		enc->mrk = 0;
		tilehdrlen = jas_stream_getrwcount(enc->tmpstream);
		assert(tilehdrlen >= 0);

/************************************************************************/
/************************************************************************/
/************************************************************************/

		if (jpc_enc_enccblks(enc)) {
			abort();
			return -1;
		}

		cp = enc->cp;
		rho = (double) (tile->brx - tile->tlx) * (tile->bry - tile->tly) /
		  ((cp->refgrdwidth - cp->imgareatlx) * (cp->refgrdheight -
		  cp->imgareatly));
		tile->rawsize = cp->rawsize * rho;

		for (lyrno = 0; lyrno < tile->numlyrs - 1; ++lyrno) {
			tile->lyrsizes[lyrno] = tile->rawsize * jpc_fixtodbl(
			  cp->tcp.ilyrrates[lyrno]);
		}
#if !defined(__clang__)
		// WARNING:
		// Some versions of Clang (e.g., 3.7.1 and 3.8.1) appear to generate
		// incorrect code for the following line.
		tile->lyrsizes[tile->numlyrs - 1] =
		  (cp->totalsize != UINT_FAST32_MAX) ?
		  (rho * enc->mainbodysize) : UINT_FAST32_MAX;
#else
		if (cp->totalsize != UINT_FAST32_MAX) {
			tile->lyrsizes[tile->numlyrs - 1] = (rho * enc->mainbodysize);
		} else {
			tile->lyrsizes[tile->numlyrs - 1] = UINT_FAST32_MAX;
		}
#endif
//jas_eprintf("TESTING %ld %ld\n", cp->totalsize != UINT_FAST32_MAX, tile->lyrsizes[0]);
		for (lyrno = 0; lyrno < tile->numlyrs; ++lyrno) {
			if (tile->lyrsizes[lyrno] != UINT_FAST32_MAX) {
				if (JAS_CAST(uint_fast32_t, tilehdrlen) <= tile->lyrsizes[lyrno]) {
					tile->lyrsizes[lyrno] -= tilehdrlen;
				} else {
					tile->lyrsizes[lyrno] = 0;
				}
			}
		}

		if (rateallocate(enc, tile->numlyrs, tile->lyrsizes)) {
			return -1;
		}

#if 0
jas_eprintf("ENCODE TILE DATA\n");
#endif
		if (jpc_enc_encodetiledata(enc)) {
			jas_eprintf("dotile failed\n");
			return -1;
		}

/************************************************************************/
/************************************************************************/
/************************************************************************/

/************************************************************************/
/************************************************************************/
/************************************************************************/

		tilelen = jas_stream_tell(enc->tmpstream);

		if (jas_stream_seek(enc->tmpstream, 6, SEEK_SET) < 0) {
			return -1;
		}
		jpc_putuint32(enc->tmpstream, tilelen);

		if (jas_stream_seek(enc->tmpstream, 0, SEEK_SET) < 0) {
			return -1;
		}
		if (jpc_putdata(enc->out, enc->tmpstream, -1)) {
			return -1;
		}
		enc->len += tilelen;

		jas_stream_close(enc->tmpstream);
		enc->tmpstream = 0;

		jpc_enc_tile_destroy(enc->curtile);
		enc->curtile = 0;

	}

	return 0;
}