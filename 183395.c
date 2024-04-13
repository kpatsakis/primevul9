Bool unRLE_obuf_to_output_SMALL ( DState* s )
{
   UChar k1;

/*    if (s->blockRandomised) { */

/*       while (True) { */
/*          /\* try to finish existing run *\/ */
/*          while (True) { */
/*             if (s->strm->avail_out == 0) return False; */
/*             if (s->state_out_len == 0) break; */
/*             *( (UChar*)(s->strm->next_out) ) = s->state_out_ch; */
/* 	       BZ_UPDATE_CRC ( s->calculatedBlockCRC, s->state_out_ch ); */
/*             s->state_out_len--; */
/*             s->strm->next_out++; */
/*             s->strm->avail_out--; */
/*             s->strm->total_out_lo32++; */
/*             if (s->strm->total_out_lo32 == 0) s->strm->total_out_hi32++; */
/*          } */
   
/*          /\* can a new run be started? *\/ */
/*          if (s->nblock_used == s->save_nblock+1) return False; */

/*          /\* Only caused by corrupt data stream? *\/ */
/*          if (s->nblock_used > s->save_nblock+1) */
/*             return True; */
   
/*          s->state_out_len = 1; */
/*          s->state_out_ch = s->k0; */
/*          BZ_GET_SMALL(k1); BZ_RAND_UPD_MASK;  */
/*          k1 ^= BZ_RAND_MASK; s->nblock_used++; */
/*          if (s->nblock_used == s->save_nblock+1) continue; */
/*          if (k1 != s->k0) { s->k0 = k1; continue; }; */
   
/*          s->state_out_len = 2; */
/*          BZ_GET_SMALL(k1); BZ_RAND_UPD_MASK;  */
/*          k1 ^= BZ_RAND_MASK; s->nblock_used++; */
/*          if (s->nblock_used == s->save_nblock+1) continue; */
/*          if (k1 != s->k0) { s->k0 = k1; continue; }; */
   
/*          s->state_out_len = 3; */
/*          BZ_GET_SMALL(k1); BZ_RAND_UPD_MASK;  */
/*          k1 ^= BZ_RAND_MASK; s->nblock_used++; */
/*          if (s->nblock_used == s->save_nblock+1) continue; */
/*          if (k1 != s->k0) { s->k0 = k1; continue; }; */
   
/*          BZ_GET_SMALL(k1); BZ_RAND_UPD_MASK;  */
/*          k1 ^= BZ_RAND_MASK; s->nblock_used++; */
/*          s->state_out_len = ((Int32)k1) + 4; */
/*          BZ_GET_SMALL(s->k0); BZ_RAND_UPD_MASK;  */
/*          s->k0 ^= BZ_RAND_MASK; s->nblock_used++; */
/*       } */

/*    } else */ {

      while (True) {
         /* try to finish existing run */
         while (True) {
            if (s->strm->avail_out == 0) return False;
            if (s->state_out_len == 0) break;
            *(s->strm->next_out) = s->state_out_ch;
	    /* aCaB BZ_UPDATE_CRC ( s->calculatedBlockCRC, s->state_out_ch ); */
            s->state_out_len--;
            s->strm->next_out++;
            s->strm->avail_out--;
            s->strm->total_out_lo32++;
            if (s->strm->total_out_lo32 == 0) s->strm->total_out_hi32++;
         }
   
         /* can a new run be started? */
         if (s->nblock_used == s->save_nblock+1) return False;

         /* Only caused by corrupt data stream? */
         if (s->nblock_used > s->save_nblock+1)
            return True;
   
         s->state_out_len = 1;
         s->state_out_ch = s->k0;
         BZ_GET_SMALL(k1); s->nblock_used++;
         if (s->nblock_used == s->save_nblock+1) continue;
         if (k1 != s->k0) { s->k0 = k1; continue; };
   
         s->state_out_len = 2;
         BZ_GET_SMALL(k1); s->nblock_used++;
         if (s->nblock_used == s->save_nblock+1) continue;
         if (k1 != s->k0) { s->k0 = k1; continue; };
   
         s->state_out_len = 3;
         BZ_GET_SMALL(k1); s->nblock_used++;
         if (s->nblock_used == s->save_nblock+1) continue;
         if (k1 != s->k0) { s->k0 = k1; continue; };
   
         BZ_GET_SMALL(k1); s->nblock_used++;
         s->state_out_len = ((Int32)k1) + 4;
         BZ_GET_SMALL(s->k0); s->nblock_used++;
      }

   }
}