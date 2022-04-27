ssize_t shutdown(int fd, int how)
{
    if (!in_segment) {
        return real_shutdown(fd, how);
    }
    batch_num++;
    int off = global_j << 6;

    btable[off + global_i].sysnum = 48;
    btable[off + global_i].rstatus = BENTRY_BUSY;
    btable[off + global_i].nargs = 2;
    btable[off + global_i].args[0] = fd;
    btable[off + global_i].args[1] = how;

    if (global_i == MAX_TABLE_SIZE - 1) {
        if (global_j == MAX_THREAD_NUM - 1) {
            global_j = 0;
        } else {
            global_j++;
        }
        global_i = 0;
    } else {
        global_i++;
    }

    /* assume success */
    return 0;
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
{
    if (!in_segment) {
        return real_writev(fd, iov, iovcnt);
    }
    batch_num++;

    int off, len = 0, i;
    off = global_j << 6;

    for (i = 0; i < iovcnt; i++) {
        int ll = iov[i].iov_len;
        len += iov[i].iov_len;
    }

    btable[off + global_i].sysnum = 20;
    btable[off + global_i].rstatus = BENTRY_BUSY;
    btable[off + global_i].nargs = 3;
    btable[off + global_i].args[0] = fd;
    btable[off + global_i].args[1] = (long) (iov);
    btable[off + global_i].args[2] = iovcnt;

    if (global_i == MAX_TABLE_SIZE - 1) {
        if (global_j == MAX_THREAD_NUM - 1) {
            global_j = 0;
        } else {
            global_j++;
        }
        global_i = 0;
    } else {
        global_i++;
    }

    /* assume always success */
    return len;
}