merge_public_with_secret (KBNODE pubblock, KBNODE secblock)
{
  KBNODE pub;

  assert (pubblock->pkt->pkttype == PKT_PUBLIC_KEY);
  assert (secblock->pkt->pkttype == PKT_SECRET_KEY);

  for (pub = pubblock; pub; pub = pub->next)
    {
      if (pub->pkt->pkttype == PKT_PUBLIC_KEY)
	{
	  PKT_public_key *pk = pub->pkt->pkt.public_key;
	  PKT_secret_key *sk = secblock->pkt->pkt.secret_key;
	  assert (pub == pubblock); /* Only in the first node.  */
	  /* There is nothing to compare in this case, so just replace
	   * some information.  */
	  copy_public_parts_to_secret_key (pk, sk);
	  free_public_key (pk);
	  pub->pkt->pkttype = PKT_SECRET_KEY;
	  pub->pkt->pkt.secret_key = copy_secret_key (NULL, sk);
	}
      else if (pub->pkt->pkttype == PKT_PUBLIC_SUBKEY)
	{
	  KBNODE sec;
	  PKT_public_key *pk = pub->pkt->pkt.public_key;

	  /* This is more complicated: It may happen that the sequence
	   * of the subkeys dosn't match, so we have to find the
	   * appropriate secret key.  */
	  for (sec = secblock->next; sec; sec = sec->next)
	    {
	      if (sec->pkt->pkttype == PKT_SECRET_SUBKEY)
		{
		  PKT_secret_key *sk = sec->pkt->pkt.secret_key;
		  if (!cmp_public_secret_key (pk, sk))
		    {
		      copy_public_parts_to_secret_key (pk, sk);
		      free_public_key (pk);
		      pub->pkt->pkttype = PKT_SECRET_SUBKEY;
		      pub->pkt->pkt.secret_key = copy_secret_key (NULL, sk);
		      break;
		    }
		}
	    }
	  if (!sec)
	    BUG (); /* Already checked in premerge.  */
	}
    }
}