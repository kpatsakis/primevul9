static void pblock(int d, js_Ast *block)
{
	assert(block->type == STM_BLOCK);
	pc('{'); nl();
	pstmlist(d, block->a);
	in(d); pc('}');
}