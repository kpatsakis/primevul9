asn1_der_decoding_startEnd (asn1_node element, const void *ider, int ider_len,
			    const char *name_element, int *start, int *end)
{
  asn1_node node, node_to_find, p, p2;
  int counter, len2, len3, len4, move, ris;
  unsigned char class;
  unsigned long tag;
  int indefinite, result = ASN1_DER_ERROR;
  const unsigned char *der = ider;

  node = element;

  if (node == NULL)
    return ASN1_ELEMENT_NOT_FOUND;

  node_to_find = asn1_find_node (node, name_element);

  if (node_to_find == NULL)
    return ASN1_ELEMENT_NOT_FOUND;

  if (node_to_find == node)
    {
      *start = 0;
      *end = ider_len - 1;
      return ASN1_SUCCESS;
    }

  if (node->type & CONST_OPTION)
    return ASN1_GENERIC_ERROR;

  counter = 0;
  move = DOWN;
  p = node;
  while (1)
    {
      if (p == NULL)
	return ASN1_DER_ERROR;

      ris = ASN1_SUCCESS;

      if (move != UP)
	{
	  if (p->type & CONST_SET)
	    {
	      p2 = _asn1_find_up (p);
	      if (p2 == NULL)
	        {
		  warn();
		  return ASN1_DER_ERROR;
		}

	      len2 = _asn1_strtol (p2->value, NULL, 10);
	      if (len2 == -1)
		{
		  DECR_LEN(ider_len, 2);
		  if (!der[counter] && !der[counter + 1])
		    {
		      p = p2;
		      move = UP;
		      counter += 2;
		      continue;
		    }
		  else
		    ider_len += 2;
		}
	      else if (counter == len2)
		{
		  p = p2;
		  move = UP;
		  continue;
		}
	      else if (counter > len2)
	        {
		  warn();
		  return ASN1_DER_ERROR;
		}

	      p2 = p2->down;

	      while (p2)
		{
		  if ((p2->type & CONST_SET) && (p2->type & CONST_NOT_USED))
		    {		/* CONTROLLARE */
		      ris =
			  extract_tag_der_recursive (p2, der + counter,
						 ider_len, &len2);
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
		  warn();
		  return ASN1_DER_ERROR;
		}
	    }

	  if (p == node_to_find)
	    *start = counter;

	  if (type_field (p->type) == ASN1_ETYPE_CHOICE)
	    {
	      p = p->down;
	      if (p == NULL)
	        {
		  warn();
		  return ASN1_DER_ERROR;
		}

	      ris =
		_asn1_extract_tag_der (p, der + counter, ider_len,
				       &len2);
	      if (p == node_to_find)
		*start = counter;
	    }

	  if (ris == ASN1_SUCCESS)
	    ris =
	      _asn1_extract_tag_der (p, der + counter, ider_len, &len2);
	  if (ris != ASN1_SUCCESS)
	    {
	      if (p->type & CONST_OPTION)
		{
		  p->type |= CONST_NOT_USED;
		  move = RIGHT;
		}
	      else if (p->type & CONST_DEFAULT)
		{
		  move = RIGHT;
		}
	      else
		{
		  warn();
		  return ASN1_TAG_ERROR;
		}
	    }
	  else
	    {
	      DECR_LEN(ider_len, len2);
	      counter += len2;
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
		  warn();
		  return ASN1_DER_ERROR;
		}
	      counter++;
	      move = RIGHT;
	      break;
	    case ASN1_ETYPE_BOOLEAN:
              DECR_LEN(ider_len, 2);

	      if (der[counter] != 1)
	        {
		  warn();
		  return ASN1_DER_ERROR;
		}

	      counter += 2;
	      move = RIGHT;
	      break;
	    case ASN1_ETYPE_OCTET_STRING:
	      ris = _asn1_get_octet_string (NULL, der + counter, ider_len, &len3);
	      if (ris != ASN1_SUCCESS)
	        {
		  warn();
		  return ris;
		}
              DECR_LEN(ider_len, len3);
	      counter += len3;
	      move = RIGHT;
	      break;
	    case ASN1_ETYPE_UTC_TIME:
	    case ASN1_ETYPE_GENERALIZED_TIME:
	    case ASN1_ETYPE_OBJECT_ID:
	    case ASN1_ETYPE_INTEGER:
	    case ASN1_ETYPE_ENUMERATED:
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
		  warn();
		  return ASN1_DER_ERROR;
		}

              DECR_LEN(ider_len, len3 + len2);
	      counter += len3 + len2;
	      move = RIGHT;
	      break;
	    case ASN1_ETYPE_SEQUENCE:
	    case ASN1_ETYPE_SET:
	      if (move != UP)
		{
		  len3 =
		    asn1_get_length_der (der + counter, ider_len, &len2);
		  if (len3 < -1)
		    {
  		      warn();
		      return ASN1_DER_ERROR;
		    }

                  DECR_LEN(ider_len, len2);
		  counter += len2;

		  if (len3 == 0)
		    move = RIGHT;
		  else
		    move = DOWN;
		}
	      else
		{
                  DECR_LEN(ider_len, 2);
		  if (!der[counter] && !der[counter + 1])	/* indefinite length method */
		    counter += 2;
		  else
		    ider_len += 2;
		  move = RIGHT;
		}
	      break;
	    case ASN1_ETYPE_SEQUENCE_OF:
	    case ASN1_ETYPE_SET_OF:
	      if (move != UP)
		{
		  len3 =
		    asn1_get_length_der (der + counter, ider_len, &len2);
		  if (len3 < -1)
		    {
  		      warn();
		      return ASN1_DER_ERROR;
		    }

                  DECR_LEN(ider_len, len2);
		  counter += len2;

		  if (len3 == -1)
		    {
		       DECR_LEN(ider_len, 2);
		       if (!der[counter] && !der[counter + 1])
		         counter += 2;
		       else
		         ider_len += 2;
		    }

		  if (len3)
		    {
		      p2 = p->down;
		      while ((type_field (p2->type) == ASN1_ETYPE_TAG) ||
			     (type_field (p2->type) == ASN1_ETYPE_SIZE))
			p2 = p2->right;
		      p = p2;
		    }
		}
	      else
		{
		  DECR_LEN(ider_len, 2);
		  if (!der[counter] && !der[counter + 1])	/* indefinite length method */
		    counter += 2;
		  else
		    ider_len += 2;
		}
	      move = RIGHT;
	      break;
	    case ASN1_ETYPE_ANY:
	      if (asn1_get_tag_der
		  (der + counter, ider_len, &class, &len2,
		   &tag) != ASN1_SUCCESS)
		 {
  		    warn();
		    return ASN1_DER_ERROR;
		 }
	      
	      DECR_LEN(ider_len, len2);

	      len4 =
		asn1_get_length_der (der + counter + len2,
				     ider_len, &len3);
	      if (len4 < -1)
	        {
 	          warn();
		  return ASN1_DER_ERROR;
		}

	      if (len4 != -1)
		{
		  DECR_LEN(ider_len, len3 + len4);
		  counter += len2 + len3 + len4;
		}
	      else
		{		/* indefinite length */
		  /* Check indefinite lenth method in an EXPLICIT TAG */
		  ider_len += len2; /* undo DECR_LEN */

		  if (counter == 0)
		    {
		      result = ASN1_DER_ERROR;
                      warn();
		      goto cleanup;
		    }

		  if ((p->type & CONST_TAG) && (der[counter - 1] == 0x80))
		    indefinite = 1;
		  else
		    indefinite = 0;

		  ris =
		    _asn1_get_indefinite_length_string (der + counter, ider_len, &len2);
		  if (ris != ASN1_SUCCESS)
		    {
 	              warn();
		      return ris;
		    }
		  counter += len2;
		  DECR_LEN(ider_len, len2);

		  /* Check if a couple of 0x00 are present due to an EXPLICIT TAG with
		     an indefinite length method. */
		  if (indefinite)
		    {
		      DECR_LEN(ider_len, 2);

		      if (!der[counter] && !der[counter + 1])
			counter += 2;
		      else
		        {
 	                  warn();
			  return ASN1_DER_ERROR;
			}
		    }
		}
	      move = RIGHT;
	      break;
	    default:
	      move = (move == UP) ? RIGHT : DOWN;
	      break;
	    }
	}

      if ((p == node_to_find) && (move == RIGHT))
	{
	  *end = counter - 1;
	  return ASN1_SUCCESS;
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

  warn();
  return ASN1_ELEMENT_NOT_FOUND;

cleanup:
  return result;
}