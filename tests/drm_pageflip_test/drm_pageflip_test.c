#include <xf86drm.h>
#include <xf86drmMode.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>
#include <cutils/log.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <asm/page.h>

#define LOG_TAG "drm_pageflip_test"

inline size_t roundUpToPageSize(size_t x) {
    return (x + (PAGE_SIZE-1)) & ~(PAGE_SIZE-1);
}

int main(void)
{
    struct fb_var_screeninfo info;
    struct fb_fix_screeninfo finfo;
    int fd = -1;
    char const * device = "/dev/graphics/fb0";

    fd = open(device, O_RDWR, 0);

    if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo) == -1)
        return -1;

    if (ioctl(fd, FBIOGET_VSCREENINFO, &info) == -1)
        return -1;

    info.reserved[0] = 0;
    info.reserved[1] = 0;
    info.reserved[2] = 0;
    info.xoffset = 0;
    info.yoffset = 0;
    info.activate = FB_ACTIVATE_NOW;

    /*
     * Request NUM_BUFFERS screens (at lest 2 for page flipping)
     */
    info.yres_virtual = info.yres * 2;

    if (ioctl(fd, FBIOPUT_VSCREENINFO, &info) == -1)
        return -1;

    if (ioctl(fd, FBIOGET_VSCREENINFO, &info) == -1)
        return -1;

    if ((fd = drmOpen(NULL, "platform:imx-drm:00")) < 0) {
        LOGE("drmOpen failed.");
        return 1;
    }

    size_t fbSize = roundUpToPageSize(finfo.line_length * info.yres_virtual);

    void* vaddr = mmap(0, fbSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (vaddr == MAP_FAILED) {
        LOGE("Error mapping the framebuffer (%s)", strerror(errno));
        return -errno;
    }
    memset(vaddr, 0, fbSize);

    /* Fill screen with random data */
    uint32_t* pixel = vaddr;
    uint32_t i;
    long ltime = time(NULL);
    srand(ltime);
    uint32_t color = 0x00ff00ff;
    for (i=0; i<fbSize/8; i++)
    {
        *pixel = color;
        pixel++;
    }
    color = 0xff0000ff;
    for (i=0; i<fbSize/8; i++)
    {
        *pixel = color;
        pixel++;
    }

    int runs = 0;
flip:
    sleep(2);

    /* Attempt to flip pages */
    int err;
    if (err = drmModePageFlip(fd, 0x3, 0xD, 0, NULL)) {
        if (err < 0)
            err = -err;
        LOGE("drmModePageFlip failed. (%d: %s)", err, strerror(err));
        return -errno;
    }
    runs++;
    if (runs < 5)
        goto flip;
    return 0;
}
