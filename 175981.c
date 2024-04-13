thunar_transfer_node_free (ThunarTransferNode *node)
{
  ThunarTransferNode *next;

  /* free all nodes in a row */
  while (node != NULL)
    {
      /* free all children of this node */
      thunar_transfer_node_free (node->children);

      /* determine the next node */
      next = node->next;

      /* drop the source file of this node */
      g_object_unref (node->source_file);

      /* release the resources of this node */
      g_slice_free (ThunarTransferNode, node);

      /* continue with the next node */
      node = next;
    }
}