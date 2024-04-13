static void *m_next(struct seq_file *m, void *v, loff_t *pos)
{
	struct proc_maps_private *priv = m->private;
	struct vm_area_struct *vma = v;
	struct vm_area_struct *tail_vma = priv->tail_vma;

	(*pos)++;
	if (vma && (vma != tail_vma) && vma->vm_next)
		return vma->vm_next;
	vma_stop(priv, vma);
	return (vma != tail_vma)? tail_vma: NULL;
}