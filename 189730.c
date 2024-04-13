_asn1_expand_object_id (asn1_node node)
{
  asn1_node p, p2, p3, p4, p5;
  char name_root[ASN1_MAX_NAME_SIZE], name2[2 * ASN1_MAX_NAME_SIZE + 1];
  int move, tlen;

  if (node == NULL)
    return ASN1_ELEMENT_NOT_FOUND;

  _asn1_str_cpy (name_root, sizeof (name_root), node->name);

  p = node;
  move = DOWN;

  while (!((p == node) && (move == UP)))
    {
      if (move != UP)
	{
	  if ((type_field (p->type) == ASN1_ETYPE_OBJECT_ID)
	      && (p->type & CONST_ASSIGN))
	    {
	      p2 = p->down;
	      if (p2 && (type_field (p2->type) == ASN1_ETYPE_CONSTANT))
		{
		  if (p2->value && !isdigit (p2->value[0]))
		    {
		      _asn1_str_cpy (name2, sizeof (name2), name_root);
		      _asn1_str_cat (name2, sizeof (name2), ".");
		      _asn1_str_cat (name2, sizeof (name2),
				     (char *) p2->value);
		      p3 = asn1_find_node (node, name2);
		      if (!p3
			  || (type_field (p3->type) != ASN1_ETYPE_OBJECT_ID)
			  || !(p3->type & CONST_ASSIGN))
			return ASN1_ELEMENT_NOT_FOUND;
		      _asn1_set_down (p, p2->right);
		      _asn1_remove_node (p2, 0);
		      p2 = p;
		      p4 = p3->down;
		      while (p4)
			{
			  if (type_field (p4->type) == ASN1_ETYPE_CONSTANT)
			    {
			      p5 =
				_asn1_add_single_node (ASN1_ETYPE_CONSTANT);
			      _asn1_set_name (p5, p4->name);
			      if (p4->value)
			        {
			          tlen = _asn1_strlen (p4->value);
			          if (tlen > 0)
			            _asn1_set_value (p5, p4->value, tlen + 1);
			        }
			      if (p2 == p)
				{
				  _asn1_set_right (p5, p->down);
				  _asn1_set_down (p, p5);
				}
			      else
				{
				  _asn1_set_right (p5, p2->right);
				  _asn1_set_right (p2, p5);
				}
			      p2 = p5;
			    }
			  p4 = p4->right;
			}
		      move = DOWN;
		      continue;
		    }
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


  /*******************************/
  /*       expand DEFAULT        */
  /*******************************/
  p = node;
  move = DOWN;

  while (!((p == node) && (move == UP)))
    {
      if (move != UP)
	{
	  if ((type_field (p->type) == ASN1_ETYPE_OBJECT_ID) &&
	      (p->type & CONST_DEFAULT))
	    {
	      p2 = p->down;
	      if (p2 && (type_field (p2->type) == ASN1_ETYPE_DEFAULT))
		{
		  _asn1_str_cpy (name2, sizeof (name2), name_root);
		  _asn1_str_cat (name2, sizeof (name2), ".");
		  _asn1_str_cat (name2, sizeof (name2), (char *) p2->value);
		  p3 = asn1_find_node (node, name2);
		  if (!p3 || (type_field (p3->type) != ASN1_ETYPE_OBJECT_ID)
		      || !(p3->type & CONST_ASSIGN))
		    return ASN1_ELEMENT_NOT_FOUND;
		  p4 = p3->down;
		  name2[0] = 0;
		  while (p4)
		    {
		      if (type_field (p4->type) == ASN1_ETYPE_CONSTANT)
			{
			  if (p4->value == NULL)
			    return ASN1_VALUE_NOT_FOUND;

			  if (name2[0])
			    _asn1_str_cat (name2, sizeof (name2), ".");
			  _asn1_str_cat (name2, sizeof (name2),
					 (char *) p4->value);
			}
		      p4 = p4->right;
		    }
		  tlen = strlen (name2);
		  if (tlen > 0)
		    _asn1_set_value (p2, name2, tlen + 1);
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