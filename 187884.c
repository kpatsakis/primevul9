premerge_public_with_secret (KBNODE pubblock, KBNODE secblock)
{
  KBNODE last, pub;

  assert (pubblock->pkt->pkttype == PKT_PUBLIC_KEY);
  assert (secblock->pkt->pkttype == PKT_SECRET_KEY);

  for (pub = pubblock, last = NULL; pub; last = pub, pub = pub->next)
    {
      pub->flag &= ~3; /* Reset bits 0 and 1.  */
      if (pub->pkt->pkttype == PKT_PUBLIC_SUBKEY)
	{
	  KBNODE sec;
	  PKT_public_key *pk = pub->pkt->pkt.public_key;

	  for (sec = secblock->next; sec; sec = sec->next)
	    {
	      if (sec->pkt->pkttype == PKT_SECRET_SUBKEY)
		{
		  PKT_secret_key *sk = sec->pkt->pkt.secret_key;
		  if (!cmp_public_secret_key (pk, sk))
		    {
		      if (sk->protect.s2k.mode == 1001)
			{
			  /* The secret parts are not available so
			     we can't use that key for signing etc.
			     Fix the pubkey usage */
			  pk->pubkey_usage &= ~(PUBKEY_USAGE_SIG
						| PUBKEY_USAGE_AUTH);
			}
		      /* Transfer flag bits 0 and 1 to the pubblock.  */
		      pub->flag |= (sec->flag & 3);
		      break;
		    }
		}
	    }
	  if (!sec)
	    {
	      KBNODE next, ll;

	      if (opt.verbose)
		log_info (_("no secret subkey"
			    " for public subkey %s - ignoring\n"),
			  keystr_from_pk (pk));
	      /* We have to remove the subkey in this case.  */
	      assert (last);
	      /* Find the next subkey.  */
	      for (next = pub->next, ll = pub;
		   next && next->pkt->pkttype != PKT_PUBLIC_SUBKEY;
		   ll = next, next = next->next)
		;
	      /* Make new link.  */
	      last->next = next;
	      /* Release this public subkey with all sigs.  */
	      ll->next = NULL;
	      release_kbnode (pub);
	      /* Let the loop continue.  */
	      pub = last;
	    }
	}
    }
  /* We need to copy the found bits (0 and 1) from the secret key to
     the public key.  This has already been done for the subkeys but
     got lost on the primary key - fix it here.  */
  pubblock->flag |= (secblock->flag & 3);
}