static int irda_setsockopt(struct socket *sock, int level, int optname,
			   char __user *optval, unsigned int optlen)
{
	struct sock *sk = sock->sk;
	struct irda_sock *self = irda_sk(sk);
	struct irda_ias_set    *ias_opt;
	struct ias_object      *ias_obj;
	struct ias_attrib *	ias_attr;	/* Attribute in IAS object */
	int opt, free_ias = 0, err = 0;

	pr_debug("%s(%p)\n", __func__, self);

	if (level != SOL_IRLMP)
		return -ENOPROTOOPT;

	lock_sock(sk);

	switch (optname) {
	case IRLMP_IAS_SET:
		/* The user want to add an attribute to an existing IAS object
		 * (in the IAS database) or to create a new object with this
		 * attribute.
		 * We first query IAS to know if the object exist, and then
		 * create the right attribute...
		 */

		if (optlen != sizeof(struct irda_ias_set)) {
			err = -EINVAL;
			goto out;
		}

		/* Copy query to the driver. */
		ias_opt = memdup_user(optval, optlen);
		if (IS_ERR(ias_opt)) {
			err = PTR_ERR(ias_opt);
			goto out;
		}

		/* Find the object we target.
		 * If the user gives us an empty string, we use the object
		 * associated with this socket. This will workaround
		 * duplicated class name - Jean II */
		if(ias_opt->irda_class_name[0] == '\0') {
			if(self->ias_obj == NULL) {
				kfree(ias_opt);
				err = -EINVAL;
				goto out;
			}
			ias_obj = self->ias_obj;
		} else
			ias_obj = irias_find_object(ias_opt->irda_class_name);

		/* Only ROOT can mess with the global IAS database.
		 * Users can only add attributes to the object associated
		 * with the socket they own - Jean II */
		if((!capable(CAP_NET_ADMIN)) &&
		   ((ias_obj == NULL) || (ias_obj != self->ias_obj))) {
			kfree(ias_opt);
			err = -EPERM;
			goto out;
		}

		/* If the object doesn't exist, create it */
		if(ias_obj == (struct ias_object *) NULL) {
			/* Create a new object */
			ias_obj = irias_new_object(ias_opt->irda_class_name,
						   jiffies);
			if (ias_obj == NULL) {
				kfree(ias_opt);
				err = -ENOMEM;
				goto out;
			}
			free_ias = 1;
		}

		/* Do we have the attribute already ? */
		if(irias_find_attrib(ias_obj, ias_opt->irda_attrib_name)) {
			kfree(ias_opt);
			if (free_ias) {
				kfree(ias_obj->name);
				kfree(ias_obj);
			}
			err = -EINVAL;
			goto out;
		}

		/* Look at the type */
		switch(ias_opt->irda_attrib_type) {
		case IAS_INTEGER:
			/* Add an integer attribute */
			irias_add_integer_attrib(
				ias_obj,
				ias_opt->irda_attrib_name,
				ias_opt->attribute.irda_attrib_int,
				IAS_USER_ATTR);
			break;
		case IAS_OCT_SEQ:
			/* Check length */
			if(ias_opt->attribute.irda_attrib_octet_seq.len >
			   IAS_MAX_OCTET_STRING) {
				kfree(ias_opt);
				if (free_ias) {
					kfree(ias_obj->name);
					kfree(ias_obj);
				}

				err = -EINVAL;
				goto out;
			}
			/* Add an octet sequence attribute */
			irias_add_octseq_attrib(
			      ias_obj,
			      ias_opt->irda_attrib_name,
			      ias_opt->attribute.irda_attrib_octet_seq.octet_seq,
			      ias_opt->attribute.irda_attrib_octet_seq.len,
			      IAS_USER_ATTR);
			break;
		case IAS_STRING:
			/* Should check charset & co */
			/* Check length */
			/* The length is encoded in a __u8, and
			 * IAS_MAX_STRING == 256, so there is no way
			 * userspace can pass us a string too large.
			 * Jean II */
			/* NULL terminate the string (avoid troubles) */
			ias_opt->attribute.irda_attrib_string.string[ias_opt->attribute.irda_attrib_string.len] = '\0';
			/* Add a string attribute */
			irias_add_string_attrib(
				ias_obj,
				ias_opt->irda_attrib_name,
				ias_opt->attribute.irda_attrib_string.string,
				IAS_USER_ATTR);
			break;
		default :
			kfree(ias_opt);
			if (free_ias) {
				kfree(ias_obj->name);
				kfree(ias_obj);
			}
			err = -EINVAL;
			goto out;
		}
		irias_insert_object(ias_obj);
		kfree(ias_opt);
		break;
	case IRLMP_IAS_DEL:
		/* The user want to delete an object from our local IAS
		 * database. We just need to query the IAS, check is the
		 * object is not owned by the kernel and delete it.
		 */

		if (optlen != sizeof(struct irda_ias_set)) {
			err = -EINVAL;
			goto out;
		}

		/* Copy query to the driver. */
		ias_opt = memdup_user(optval, optlen);
		if (IS_ERR(ias_opt)) {
			err = PTR_ERR(ias_opt);
			goto out;
		}

		/* Find the object we target.
		 * If the user gives us an empty string, we use the object
		 * associated with this socket. This will workaround
		 * duplicated class name - Jean II */
		if(ias_opt->irda_class_name[0] == '\0')
			ias_obj = self->ias_obj;
		else
			ias_obj = irias_find_object(ias_opt->irda_class_name);
		if(ias_obj == (struct ias_object *) NULL) {
			kfree(ias_opt);
			err = -EINVAL;
			goto out;
		}

		/* Only ROOT can mess with the global IAS database.
		 * Users can only del attributes from the object associated
		 * with the socket they own - Jean II */
		if((!capable(CAP_NET_ADMIN)) &&
		   ((ias_obj == NULL) || (ias_obj != self->ias_obj))) {
			kfree(ias_opt);
			err = -EPERM;
			goto out;
		}

		/* Find the attribute (in the object) we target */
		ias_attr = irias_find_attrib(ias_obj,
					     ias_opt->irda_attrib_name);
		if(ias_attr == (struct ias_attrib *) NULL) {
			kfree(ias_opt);
			err = -EINVAL;
			goto out;
		}

		/* Check is the user space own the object */
		if(ias_attr->value->owner != IAS_USER_ATTR) {
			pr_debug("%s(), attempting to delete a kernel attribute\n",
				 __func__);
			kfree(ias_opt);
			err = -EPERM;
			goto out;
		}

		/* Remove the attribute (and maybe the object) */
		irias_delete_attrib(ias_obj, ias_attr, 1);
		kfree(ias_opt);
		break;
	case IRLMP_MAX_SDU_SIZE:
		if (optlen < sizeof(int)) {
			err = -EINVAL;
			goto out;
		}

		if (get_user(opt, (int __user *)optval)) {
			err = -EFAULT;
			goto out;
		}

		/* Only possible for a seqpacket service (TTP with SAR) */
		if (sk->sk_type != SOCK_SEQPACKET) {
			pr_debug("%s(), setting max_sdu_size = %d\n",
				 __func__, opt);
			self->max_sdu_size_rx = opt;
		} else {
			net_warn_ratelimited("%s: not allowed to set MAXSDUSIZE for this socket type!\n",
					     __func__);
			err = -ENOPROTOOPT;
			goto out;
		}
		break;
	case IRLMP_HINTS_SET:
		if (optlen < sizeof(int)) {
			err = -EINVAL;
			goto out;
		}

		/* The input is really a (__u8 hints[2]), easier as an int */
		if (get_user(opt, (int __user *)optval)) {
			err = -EFAULT;
			goto out;
		}

		/* Unregister any old registration */
		irlmp_unregister_service(self->skey);

		self->skey = irlmp_register_service((__u16) opt);
		break;
	case IRLMP_HINT_MASK_SET:
		/* As opposed to the previous case which set the hint bits
		 * that we advertise, this one set the filter we use when
		 * making a discovery (nodes which don't match any hint
		 * bit in the mask are not reported).
		 */
		if (optlen < sizeof(int)) {
			err = -EINVAL;
			goto out;
		}

		/* The input is really a (__u8 hints[2]), easier as an int */
		if (get_user(opt, (int __user *)optval)) {
			err = -EFAULT;
			goto out;
		}

		/* Set the new hint mask */
		self->mask.word = (__u16) opt;
		/* Mask out extension bits */
		self->mask.word &= 0x7f7f;
		/* Check if no bits */
		if(!self->mask.word)
			self->mask.word = 0xFFFF;

		break;
	default:
		err = -ENOPROTOOPT;
		break;
	}

out:
	release_sock(sk);

	return err;
}