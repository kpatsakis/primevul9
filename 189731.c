_asn1_append_value (asn1_node node, const void *value, unsigned int len)
{
  if (node == NULL)
    return node;

  if (node->value == NULL)
    return _asn1_set_value (node, value, len);

  if (len == 0)
    return node;

  if (node->value == node->small_value)
    {
      /* value is in node */
      int prev_len = node->value_len;
      node->value_len += len;
      node->value = malloc (node->value_len);
      if (node->value == NULL)
	{
	  node->value_len = 0;
	  return NULL;
	}

      if (prev_len > 0)
        memcpy (node->value, node->small_value, prev_len);

      memcpy (&node->value[prev_len], value, len);

      return node;
    }
  else /* if (node->value != NULL && node->value != node->small_value) */
    {
      /* value is allocated */
      int prev_len = node->value_len;
      node->value_len += len;

      node->value = _asn1_realloc (node->value, node->value_len);
      if (node->value == NULL)
	{
	  node->value_len = 0;
	  return NULL;
	}

      memcpy (&node->value[prev_len], value, len);

      return node;
    }
}