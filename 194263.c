asn1_der_decoding2 (asn1_node *element, const void *ider, int *max_ider_len,
		    unsigned int flags, char *errorDescription)
{
  asn1_node node, p, p2, p3;
  char temp[128];
  int counter, len2, len3, len4, move, ris, tlen;
  struct node_tail_cache_st tcache = {NULL, NULL};
  unsigned char class;
  unsigned long tag;
  int tag_len;
  int indefinite, result, total_len = *max_ider_len, ider_len = *max_ider_len;
  int inner_tag_len;
  unsigned char *ptmp;
  const unsigned char *ptag;
  const unsigned char *der = ider;

  node = *element;

  if (errorDescription != NULL)
    errorDescription[0] = 0;

  if (node == NULL)
    return ASN1_ELEMENT_NOT_FOUND;

  if (node->type & CONST_OPTION)
    {
      result = ASN1_GENERIC_ERROR;
      warn();
      goto cleanup;
    }

  counter = 0;
  move = DOWN;
  p = node;
  while (1)
    {
      tag_len = 0;
      inner_tag_len = 0;
      ris = ASN1_SUCCESS;
      if (move != UP)
	{
	  if (p->type & CONST_SET)
	    {
	      p2 = _asn1_find_up (p);
	      len2 = p2->tmp_ival;
	      if (len2 == -1)
		{
		  if (HAVE_TWO(ider_len) && !der[counter] && !der[counter + 1])
		    {
		      p = p2;
		      move = UP;
		      counter += 2;
		      DECR_LEN(ider_len, 2);
		      continue;
		    }
		}
	      else if (counter == len2)
		{
		  p = p2;
		  move = UP;
		  continue;
		}
	      else if (counter > len2)
		{
		  result = ASN1_DER_ERROR;
                  warn();
		  goto cleanup;
		}
	      p2 = p2->down;
	      while (p2)
		{
		  if ((p2->type & CONST_SET) && (p2->type & CONST_NOT_USED))
		    {
		      ris =
			  extract_tag_der_recursive (p2, der + counter,
						     ider_len, &len2, NULL, flags);
		      if (ris == ASN1_SUCCESS)
			{
			  p2->type &= ~CONST_NOT_USED;
			  p = p2;
			  break;
			}
		    }
		  p2 = p2->right;
		}
	      if (p2 == NULL)
		{
		  result = ASN1_DER_ERROR;
                  warn();
		  goto cleanup;
		}
	    }

	  /* the position in the DER structure this starts */
	  p->start = counter;
	  p->end = total_len - 1;

	  if ((p->type & CONST_OPTION) || (p->type & CONST_DEFAULT))
	    {
	      p2 = _asn1_find_up (p);
	      len2 = p2->tmp_ival;
	      if (counter == len2)
		{
		  if (p->right)
		    {
		      p2 = p->right;
		      move = RIGHT;
		    }
		  else
		    move = UP;

		  if (p->type & CONST_OPTION)
		    asn1_delete_structure (&p);

		  p = p2;
		  continue;
		}
	    }

	  if (type_field (p->type) == ASN1_ETYPE_CHOICE)
	    {
	      while (p->down)
		{
		  ris =
		      extract_tag_der_recursive (p->down, der + counter,
					         ider_len, &len2, NULL, flags);

		  if (ris == ASN1_SUCCESS)
		    {
		      delete_unneeded_choice_fields(p->down);
		      break;
		    }
		  else if (ris == ASN1_ERROR_TYPE_ANY)
		    {
		      result = ASN1_ERROR_TYPE_ANY;
                      warn();
		      goto cleanup;
		    }
		  else
		    {
		      p2 = p->down;
		      asn1_delete_structure (&p2);
		    }
		}

	      if (p->down == NULL)
		{
		  if (!(p->type & CONST_OPTION))
		    {
		      result = ASN1_DER_ERROR;
                      warn();
		      goto cleanup;
		    }
		}
	      else if (type_field (p->type) != ASN1_ETYPE_CHOICE)
		p = p->down;

	      p->start = counter;
	    }

	  if ((p->type & CONST_OPTION) || (p->type & CONST_DEFAULT))
	    {
	      p2 = _asn1_find_up (p);
	      len2 = p2->tmp_ival;

	      if ((len2 != -1) && (counter > len2))
		ris = ASN1_TAG_ERROR;
	    }

	  if (ris == ASN1_SUCCESS)
	    ris =
	      extract_tag_der_recursive (p, der + counter, ider_len, 
	                                 &tag_len, &inner_tag_len, flags);

	  if (ris != ASN1_SUCCESS)
	    {
	      if (p->type & CONST_OPTION)
		{
		  p->type |= CONST_NOT_USED;
		  move = RIGHT;
		}
	      else if (p->type & CONST_DEFAULT)
		{
		  _asn1_set_value (p, NULL, 0);
		  move = RIGHT;
		}
	      else
		{
		  if (errorDescription != NULL)
		    _asn1_error_description_tag_error (p, errorDescription);

		  result = ASN1_TAG_ERROR;
                  warn();
		  goto cleanup;
		}
	    }
	  else
	    {
	      DECR_LEN(ider_len, tag_len);
	      counter += tag_len;
	    }
	}

      if (ris == ASN1_SUCCESS)
	{
	  switch (type_field (p->type))
	    {
	    case ASN1_ETYPE_NULL:
	      DECR_LEN(ider_len, 1);
	      if (der[counter])
		{
		  result = ASN1_DER_ERROR;
                  warn();
		  goto cleanup;
		}
	      counter++;
	      move = RIGHT;
	      break;
	    case ASN1_ETYPE_BOOLEAN:
	      DECR_LEN(ider_len, 2);

	      if (der[counter++] != 1)
		{
		  result = ASN1_DER_ERROR;
                  warn();
		  goto cleanup;
		}
	      if (der[counter++] == 0)
		_asn1_set_value (p, "F", 1);
	      else
		_asn1_set_value (p, "T", 1);
	      move = RIGHT;
	      break;
	    case ASN1_ETYPE_INTEGER:
	    case ASN1_ETYPE_ENUMERATED:
	      len2 =
		asn1_get_length_der (der + counter, ider_len, &len3);
	      if (len2 < 0)
		{
		  result = ASN1_DER_ERROR;
                  warn();
		  goto cleanup;
		}

	      DECR_LEN(ider_len, len3+len2);

	      _asn1_set_value (p, der + counter, len3 + len2);
	      counter += len3 + len2;
	      move = RIGHT;
	      break;
	    case ASN1_ETYPE_OBJECT_ID:
	      result =
		asn1_get_object_id_der (der + counter, ider_len, &len2,
					temp, sizeof (temp));
	      if (result != ASN1_SUCCESS)
	        {
                  warn();
		  goto cleanup;
		}

	      DECR_LEN(ider_len, len2);

	      tlen = strlen (temp);
	      if (tlen > 0)
		_asn1_set_value (p, temp, tlen + 1);

	      counter += len2;
	      move = RIGHT;
	      break;
	    case ASN1_ETYPE_GENERALIZED_TIME:
	    case ASN1_ETYPE_UTC_TIME:
	      result =
		_asn1_get_time_der (type_field (p->type), der + counter, ider_len, &len2, temp,
				    sizeof (temp) - 1, flags);
	      if (result != ASN1_SUCCESS)
	        {
                  warn();
                  goto cleanup;
                }

	      DECR_LEN(ider_len, len2);

	      tlen = strlen (temp);
	      if (tlen > 0)
		_asn1_set_value (p, temp, tlen);

	      counter += len2;
	      move = RIGHT;
	      break;
	    case ASN1_ETYPE_OCTET_STRING:
	      if (counter < inner_tag_len)
	        {
		  result = ASN1_DER_ERROR;
                  warn();
		  goto cleanup;
	        }

              ptag = der + counter - inner_tag_len;
              if (flags & ASN1_DECODE_FLAG_STRICT_DER || !(ptag[0] & ASN1_CLASS_STRUCTURED))
                {
	          len2 =
		    asn1_get_length_der (der + counter, ider_len, &len3);
	          if (len2 < 0)
		    {
		      result = ASN1_DER_ERROR;
                      warn();
		      goto cleanup;
		    }

	          DECR_LEN(ider_len, len3+len2);

	          _asn1_set_value (p, der + counter, len3 + len2);
	          counter += len3 + len2;
                }
              else
                {
                  unsigned dflags = 0, vlen, ber_len;

                  if (ptag[0] & ASN1_CLASS_STRUCTURED)
                    dflags |= DECODE_FLAG_INDEFINITE;

                  result = _asn1_decode_simple_ber(type_field (p->type), der+counter, ider_len, &ptmp, &vlen, &ber_len, dflags);
                  if (result != ASN1_SUCCESS)
	            {
                      warn();
		      goto cleanup;
		    }

		  DECR_LEN(ider_len, ber_len);

		  _asn1_set_value_lv (p, ptmp, vlen);

	          counter += ber_len;
	          free(ptmp);
                }
	      move = RIGHT;
	      break;
	    case ASN1_ETYPE_GENERALSTRING:
	    case ASN1_ETYPE_NUMERIC_STRING:
	    case ASN1_ETYPE_IA5_STRING:
	    case ASN1_ETYPE_TELETEX_STRING:
	    case ASN1_ETYPE_PRINTABLE_STRING:
	    case ASN1_ETYPE_UNIVERSAL_STRING:
	    case ASN1_ETYPE_BMP_STRING:
	    case ASN1_ETYPE_UTF8_STRING:
	    case ASN1_ETYPE_VISIBLE_STRING:
	    case ASN1_ETYPE_BIT_STRING:
	      len2 =
		asn1_get_length_der (der + counter, ider_len, &len3);
	      if (len2 < 0)
		{
		  result = ASN1_DER_ERROR;
                  warn();
		  goto cleanup;
		}

	      DECR_LEN(ider_len, len3+len2);

	      _asn1_set_value (p, der + counter, len3 + len2);
	      counter += len3 + len2;
	      move = RIGHT;
	      break;
	    case ASN1_ETYPE_SEQUENCE:
	    case ASN1_ETYPE_SET:
	      if (move == UP)
		{
		  len2 = p->tmp_ival;
		  p->tmp_ival = 0;
		  if (len2 == -1)
		    {		/* indefinite length method */
		      DECR_LEN(ider_len, 2);
		      if ((der[counter]) || der[counter + 1])
		        {
		          result = ASN1_DER_ERROR;
                          warn();
		          goto cleanup;
			}
		      counter += 2;
		    }
		  else
		    {		/* definite length method */
		      if (len2 != counter)
			{
			  result = ASN1_DER_ERROR;
                          warn();
			  goto cleanup;
			}
		    }
		  move = RIGHT;
		}
	      else
		{		/* move==DOWN || move==RIGHT */
		  len3 =
		    asn1_get_length_der (der + counter, ider_len, &len2);
                  if (IS_ERR(len3, flags))
		    {
		      result = ASN1_DER_ERROR;
                      warn();
		      goto cleanup;
		    }

	          DECR_LEN(ider_len, len2);
		  counter += len2;

		  if (len3 > 0)
		    {
		      p->tmp_ival = counter + len3;
		      move = DOWN;
		    }
		  else if (len3 == 0)
		    {
		      p2 = p->down;
		      while (p2)
			{
			  if (type_field (p2->type) != ASN1_ETYPE_TAG)
			    {
			      p3 = p2->right;
			      asn1_delete_structure (&p2);
			      p2 = p3;
			    }
			  else
			    p2 = p2->right;
			}
		      move = RIGHT;
		    }
		  else
		    {		/* indefinite length method */
		      p->tmp_ival = -1;
		      move = DOWN;
		    }
		}
	      break;
	    case ASN1_ETYPE_SEQUENCE_OF:
	    case ASN1_ETYPE_SET_OF:
	      if (move == UP)
		{
		  len2 = p->tmp_ival;
		  if (len2 == -1)
		    {		/* indefinite length method */
		      if (!HAVE_TWO(ider_len) || ((der[counter]) || der[counter + 1]))
			{
			  result = _asn1_append_sequence_set (p, &tcache);
			  if (result != 0)
			    {
                              warn();
		              goto cleanup;
		            }
			  p = tcache.tail;
			  move = RIGHT;
			  continue;
			}

		      p->tmp_ival = 0;
		      tcache.tail = NULL; /* finished decoding this structure */
		      tcache.head = NULL;
		      DECR_LEN(ider_len, 2);
		      counter += 2;
		    }
		  else
		    {		/* definite length method */
		      if (len2 > counter)
			{
			  result = _asn1_append_sequence_set (p, &tcache);
			  if (result != 0)
			    {
                              warn();
		              goto cleanup;
		            }
			  p = tcache.tail;
			  move = RIGHT;
			  continue;
			}

		      p->tmp_ival = 0;
		      tcache.tail = NULL; /* finished decoding this structure */
		      tcache.head = NULL;

		      if (len2 != counter)
			{
			  result = ASN1_DER_ERROR;
                          warn();
			  goto cleanup;
			}
		    }
		}
	      else
		{		/* move==DOWN || move==RIGHT */
		  len3 =
		    asn1_get_length_der (der + counter, ider_len, &len2);
                  if (IS_ERR(len3, flags))
		    {
		      result = ASN1_DER_ERROR;
                      warn();
		      goto cleanup;
		    }

		  DECR_LEN(ider_len, len2);
		  counter += len2;
		  if (len3)
		    {
		      if (len3 > 0)
			{	/* definite length method */
		          p->tmp_ival = counter + len3;
			}
		      else
			{	/* indefinite length method */
		          p->tmp_ival = -1;
			}

		      p2 = p->down;
                      if (p2 == NULL)
		        {
		          result = ASN1_DER_ERROR;
                          warn();
		          goto cleanup;
		        }

		      while ((type_field (p2->type) == ASN1_ETYPE_TAG)
			     || (type_field (p2->type) == ASN1_ETYPE_SIZE))
			p2 = p2->right;
		      if (p2->right == NULL)
		        {
			  result = _asn1_append_sequence_set (p, &tcache);
			  if (result != 0)
			    {
                              warn();
		              goto cleanup;
		            }
			}
		      p = p2;
		    }
		}
	      move = RIGHT;
	      break;
	    case ASN1_ETYPE_ANY:
	      /* Check indefinite lenth method in an EXPLICIT TAG */
              
	      if (!(flags & ASN1_DECODE_FLAG_STRICT_DER) && (p->type & CONST_TAG) && 
	          tag_len == 2 && (der[counter - 1] == 0x80))
		indefinite = 1;
	      else
	        indefinite = 0;

	      if (asn1_get_tag_der
		  (der + counter, ider_len, &class, &len2,
		   &tag) != ASN1_SUCCESS)
		{
		  result = ASN1_DER_ERROR;
                  warn();
		  goto cleanup;
		}

	      DECR_LEN(ider_len, len2);

	      len4 =
		asn1_get_length_der (der + counter + len2,
				     ider_len, &len3);
              if (IS_ERR(len4, flags))
		{
		  result = ASN1_DER_ERROR;
                  warn();
		  goto cleanup;
		}
	      if (len4 != -1) /* definite */
		{
		  len2 += len4;

	          DECR_LEN(ider_len, len4+len3);
		  _asn1_set_value_lv (p, der + counter, len2 + len3);
		  counter += len2 + len3;
		}
	      else /* == -1 */
		{		/* indefinite length */
		  ider_len += len2; /* undo DECR_LEN */

		  if (counter == 0)
		    {
		      result = ASN1_DER_ERROR;
                      warn();
		      goto cleanup;
		    }

		  result =
		    _asn1_get_indefinite_length_string (der + counter, ider_len, &len2);
		  if (result != ASN1_SUCCESS)
		    {
                      warn();
                      goto cleanup;
                    }

	          DECR_LEN(ider_len, len2);
		  _asn1_set_value_lv (p, der + counter, len2);
		  counter += len2;

		}

	        /* Check if a couple of 0x00 are present due to an EXPLICIT TAG with
	           an indefinite length method. */
	        if (indefinite)
		  {
	            DECR_LEN(ider_len, 2);
		    if (!der[counter] && !der[counter + 1])
		      {
		        counter += 2;
		      }
		    else
		      {
		        result = ASN1_DER_ERROR;
                        warn();
		        goto cleanup;
		      }
		  }

	      move = RIGHT;
	      break;
	    default:
	      move = (move == UP) ? RIGHT : DOWN;
	      break;
	    }
	}

      if (p)
        {
          p->end = counter - 1;
        }

      if (p == node && move != DOWN)
	break;

      if (move == DOWN)
	{
	  if (p->down)
	    p = p->down;
	  else
	    move = RIGHT;
	}
      if ((move == RIGHT) && !(p->type & CONST_SET))
	{
	  if (p->right)
	    p = p->right;
	  else
	    move = UP;
	}
      if (move == UP)
	p = _asn1_find_up (p);
    }

  _asn1_delete_not_used (*element);

  if ((ider_len < 0) ||
      (!(flags & ASN1_DECODE_FLAG_ALLOW_PADDING) && (ider_len != 0)))
    {
      warn();
      result = ASN1_DER_ERROR;
      goto cleanup;
    }

  *max_ider_len = total_len - ider_len;

  return ASN1_SUCCESS;

cleanup:
  asn1_delete_structure (element);
  return result;
}