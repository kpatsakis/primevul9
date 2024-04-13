static struct socket *vhost_net_stop_vq(struct vhost_net *n,
					struct vhost_virtqueue *vq)
{
	struct socket *sock;

	mutex_lock(&vq->mutex);
	sock = rcu_dereference_protected(vq->private_data,
					 lockdep_is_held(&vq->mutex));
	vhost_net_disable_vq(n, vq);
	rcu_assign_pointer(vq->private_data, NULL);
	mutex_unlock(&vq->mutex);
	return sock;
}