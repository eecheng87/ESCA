/*
 * Syscall wrapper for Effective System Call Aggregation (ESCA).
 *
 * Copyright (c) 2021-2022 National Cheng Kung University, Taiwan.
 * Authored by Yao Hwang <yaohwang99@gmail.com>
 */

int close(int fd)
{
    if (!in_segment) {
        real_close = real_close ? real_close : dlsym(RTLD_NEXT, "close");
        return real_close(fd);
    }

    batch_num++;
    int off = global_j << 6;

    btable[off + global_i].sysnum = 3;
    btable[off + global_i].rstatus = BENTRY_BUSY;
    btable[off + global_i].nargs = 1;
    btable[off + global_i].args[0] = fd;

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

    return 0;
}

ssize_t sendfile64(int outfd, int infd, off_t *offset, size_t count)
{
    if (!in_segment)
        return real_sendfile(outfd, infd, offset, count);

    batch_num++;
    int off = global_j << 6;

    btable[off + global_i].sysnum = 40;
    btable[off + global_i].rstatus = BENTRY_BUSY;
    btable[off + global_i].nargs = 4;
    btable[off + global_i].args[0] = outfd;
    btable[off + global_i].args[1] = infd;
    btable[off + global_i].args[2] = offset;
    btable[off + global_i].args[3] = count;

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
    return count;
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags)
{
    if (!in_segment)
        return real_send(sockfd, buf, len, flags);

    batch_num++;
    int off = global_j << 6;

    btable[off + global_i].sysnum = 44;
    btable[off + global_i].rstatus = BENTRY_BUSY;
    btable[off + global_i].nargs = 6;
    btable[off + global_i].args[0] = sockfd;
    btable[off + global_i].args[1] = buf;
    btable[off + global_i].args[2] = len;
    btable[off + global_i].args[3] = flags;
    btable[off + global_i].args[4] = NULL;
    btable[off + global_i].args[5] = 0;

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
