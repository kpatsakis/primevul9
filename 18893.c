static int hid_debug_events_open(struct inode *inode, struct file *file)
{
	int err = 0;
	struct hid_debug_list *list;
	unsigned long flags;

	if (!(list = kzalloc(sizeof(struct hid_debug_list), GFP_KERNEL))) {
		err = -ENOMEM;
		goto out;
	}

	if (!(list->hid_debug_buf = kzalloc(sizeof(char) * HID_DEBUG_BUFSIZE, GFP_KERNEL))) {
		err = -ENOMEM;
		kfree(list);
		goto out;
	}
	list->hdev = (struct hid_device *) inode->i_private;
	file->private_data = list;
	mutex_init(&list->read_mutex);

	spin_lock_irqsave(&list->hdev->debug_list_lock, flags);
	list_add_tail(&list->node, &list->hdev->debug_list);
	spin_unlock_irqrestore(&list->hdev->debug_list_lock, flags);

out:
	return err;
}