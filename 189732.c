_asn1_change_integer_value (asn1_node node)
{
  asn1_node p;
  unsigned char val[SIZEOF_UNSIGNED_LONG_INT];
  unsigned char val2[SIZEOF_UNSIGNED_LONG_INT + 1];
  int len;

  if (node == NULL)
    return ASN1_ELEMENT_NOT_FOUND;

  p = node;
  while (p)
    {
      if ((type_field (p->type) == ASN1_ETYPE_INTEGER)
	  && (p->type & CONST_ASSIGN))
	{
	  if (p->value)
	    {
	      _asn1_convert_integer (p->value, val, sizeof (val), &len);
	      asn1_octet_der (val, len, val2, &len);
	      _asn1_set_value (p, val2, len);
	    }
	}

      if (p->down)
	{
	  p = p->down;
	}
      else
	{
	  if (p == node)
	    p = NULL;
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
    }

  return ASN1_SUCCESS;
}