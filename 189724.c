_asn1_set_default_tag (asn1_node node)
{
  asn1_node p;

  if ((node == NULL) || (type_field (node->type) != ASN1_ETYPE_DEFINITIONS))
    return ASN1_ELEMENT_NOT_FOUND;

  p = node;
  while (p)
    {
      if ((type_field (p->type) == ASN1_ETYPE_TAG) &&
	  !(p->type & CONST_EXPLICIT) && !(p->type & CONST_IMPLICIT))
	{
	  if (node->type & CONST_EXPLICIT)
	    p->type |= CONST_EXPLICIT;
	  else
	    p->type |= CONST_IMPLICIT;
	}

      if (p->down)
	{
	  p = p->down;
	}
      else if (p->right)
	p = p->right;
      else
	{
	  while (1)
	    {
	      p = _asn1_find_up (p);
	      if (p == node)
		{
		  p = NULL;
		  break;
		}
	      if (p && p->right)
		{
		  p = p->right;
		  break;
		}
	    }
	}
    }

  return ASN1_SUCCESS;
}