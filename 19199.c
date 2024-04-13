void hint_node_free(RBNode *node, void *user) {
	free (container_of (node, HintNode, rb));
}