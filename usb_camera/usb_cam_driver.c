#include <linux/module.h>
#include <linux/usb.h>
#include <linux/slab.h>

#define USB_VENDOR_ID 0x1234
#define USB_PRODUCT_ID 0x5678

struct usb_camera {
    struct usb_device *udev;
    struct urb *urb;
    unsigned char *buffer;
    dma_addr_t dma_handle;
};

static struct usb_device_id camera_id_table[] = {
    { USB_DEVICE(USB_VENDOR_ID, USB_PRODUCT_ID) },
    { } /* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, camera_id_table);

static int camera_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    struct usb_device *udev = interface_to_usbdev(interface);
    struct usb_camera *camera;
    struct urb *urb;
    unsigned char *buffer;
    dma_addr_t dma_handle;
    int i, ret = -ENOMEM;

    camera = kzalloc(sizeof(*camera), GFP_KERNEL);
    if (!camera)
        return ret;

    camera->udev = usb_get_dev(udev);

    /* Allocate a buffer for the URB */
    buffer = usb_alloc_coherent(camera->udev, URB_SIZE, GFP_KERNEL, &dma_handle);
    if (!buffer)
        goto error;

    urb = usb_alloc_urb(0, GFP_KERNEL);
    if (!urb)
        goto error;

    /* Populate the URB */
    usb_fill_bulk_urb(urb, camera->udev, usb_rcvbulkpipe(camera->udev, EP_IN),
                      buffer, URB_SIZE, camera_complete, camera);
    urb->transfer_dma = dma_handle;
    urb->transfer_flags |= URB_DMA_MAP_SINGLE;

    /* Submit the URB */
    ret = usb_submit_urb(urb, GFP_KERNEL);
    if (ret < 0) {
        dev_err(&interface->dev, "failed to submit URB: %d\n", ret);
        usb_free_urb(urb);
        goto error;
    }

    camera->urb = urb;
    camera->buffer = buffer;
    camera->dma_handle = dma_handle;

    return 0;

error:
    if (urb)
        usb_free_urb(urb);
    if (buffer)
        usb_free_coherent(camera->udev, URB_SIZE, buffer, dma_handle);
    kfree(camera);
    return ret;
}

static void camera_disconnect(struct usb_interface *interface)
{
    struct usb_camera *camera = usb_get_intfdata(interface);

    /* Cancel the URB */
    usb_kill_urb(camera->urb);

    /* Free the URB buffer */
    usb_free_coherent(camera->udev, URB_SIZE, camera->buffer, camera->dma_handle);

    /* Free the URB */
    usb_free_urb(camera->urb);

    /* Release the USB device */
    usb_put_dev(camera->udev);

    /* Free the camera structure */
    kfree(camera);
}

static struct usb_driver camera_driver = {
    .name = "camera_driver",
    .id_table = camera_id_table,
    .probe = camera_probe,
    .disconnect = camera_disconnect,
};

static int __init camera_init(void)
{
    int ret;

    ret = usb_register(&camera_driver);
    if (ret < 0) {
        pr_err("failed to register USB driver: %d\n", ret);
        return ret;
    }

    return 0;
}

static void __exit camera_exit(void)
{
    usb_deregister(&camera_driver);
}

module_init(camera_init);
module_exit(camera_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("USB CAMERA DRIVER TEST")

