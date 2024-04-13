_asn1_type_set_config (asn1_node node)
{
  asn1_node p, p2;
  int move;

  if (node == NULL)
    return ASN1_ELEMENT_NOT_FOUND;

  p = node;
  move = DOWN;

  while (!((p == node) && (move == UP)))
    {
      if (move != UP)
	{
	  if (type_field (p->type) == ASN1_ETYPE_SET)
	    {
	      p2 = p->down;
	      while (p2)
		{
		  if (type_field (p2->type) != ASN1_ETYPE_TAG)
		    p2->type |= CONST_SET | CONST_NOT_USED;
		  p2 = p2->right;
		}
	    }
	  move = DOWN;
	}
      else
	move = RIGHT;

      if (move == DOWN)
	{
	  if (p->down)
	    p = p->down;
	  else
	    move = RIGHT;
	}

      if (p == node)
	{
	  move = UP;
	  continue;
	}

      if (move == RIGHT)
	{
	  if (p && p->right)
	    p = p->right;
	  else
	    move = UP;
	}
      if (move == UP)
	p = _asn1_find_up (p);
    }

  return ASN1_SUCCESS;
}