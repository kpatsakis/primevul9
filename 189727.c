_asn1_check_identifier (asn1_node node)
{
  asn1_node p, p2;
  char name2[ASN1_MAX_NAME_SIZE * 2 + 2];

  if (node == NULL)
    return ASN1_ELEMENT_NOT_FOUND;

  p = node;
  while (p)
    {
      if (p->value && type_field (p->type) == ASN1_ETYPE_IDENTIFIER)
	{
	  _asn1_str_cpy (name2, sizeof (name2), node->name);
	  _asn1_str_cat (name2, sizeof (name2), ".");
	  _asn1_str_cat (name2, sizeof (name2), (char *) p->value);
	  p2 = asn1_find_node (node, name2);
	  if (p2 == NULL)
	    {
	      if (p->value)
		_asn1_strcpy (_asn1_identifierMissing, p->value);
	      else
		_asn1_strcpy (_asn1_identifierMissing, "(null)");
	      return ASN1_IDENTIFIER_NOT_FOUND;
	    }
	}
      else if ((type_field (p->type) == ASN1_ETYPE_OBJECT_ID) &&
	       (p->type & CONST_DEFAULT))
	{
	  p2 = p->down;
	  if (p2 && (type_field (p2->type) == ASN1_ETYPE_DEFAULT))
	    {
	      _asn1_str_cpy (name2, sizeof (name2), node->name);
	      _asn1_str_cat (name2, sizeof (name2), ".");
	      _asn1_str_cat (name2, sizeof (name2), (char *) p2->value);
	      _asn1_strcpy (_asn1_identifierMissing, p2->value);
	      p2 = asn1_find_node (node, name2);
	      if (!p2 || (type_field (p2->type) != ASN1_ETYPE_OBJECT_ID) ||
		  !(p2->type & CONST_ASSIGN))
		return ASN1_IDENTIFIER_NOT_FOUND;
	      else
		_asn1_identifierMissing[0] = 0;
	    }
	}
      else if ((type_field (p->type) == ASN1_ETYPE_OBJECT_ID) &&
	       (p->type & CONST_ASSIGN))
	{
	  p2 = p->down;
	  if (p2 && (type_field (p2->type) == ASN1_ETYPE_CONSTANT))
	    {
	      if (p2->value && !isdigit (p2->value[0]))
		{
		  _asn1_str_cpy (name2, sizeof (name2), node->name);
		  _asn1_str_cat (name2, sizeof (name2), ".");
		  _asn1_str_cat (name2, sizeof (name2), (char *) p2->value);
		  _asn1_strcpy (_asn1_identifierMissing, p2->value);
		  p2 = asn1_find_node (node, name2);
		  if (!p2 || (type_field (p2->type) != ASN1_ETYPE_OBJECT_ID)
		      || !(p2->type & CONST_ASSIGN))
		    return ASN1_IDENTIFIER_NOT_FOUND;
		  else
		    _asn1_identifierMissing[0] = 0;
		}
	    }
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