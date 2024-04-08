#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "upgrade_format_def.h"
#include "ef_utils.h"


int main(int argc, char **argv)
{
    if (argc != 3)
        printf("invalid argc=[%d]\n", argc);

    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd < 0)
    {
        printf("invalid src file path=[%s]\n", argv[1]);
        goto src_fd_open_err;
    }
    int dest_fd = open(argv[2], O_CREAT|O_WRONLY);
    if (src_fd < 0)
    {
        printf("invalid src file path=[%s]\n", argv[1]);
        goto dest_fd_open_err;
    }

    int err = 0;
    uint32_t file_size = lseek(src_fd, 0, SEEK_END);
    uint32_t file_checksum = 0x0;
    // read src file and checksum
    lseek(src_fd, 0, SEEK_SET);
    uint32_t read_file_size = 0;
    for (;;)
    {
        static uint8_t read_buf[4096] = {0x0};
        int ret = read(src_fd, read_buf, sizeof(read_buf));
        if (ret)
        {
            read_file_size += ret;
            file_checksum = ef_calc_crc32(file_checksum, read_buf, ret);
        }
        else
        {
            if (read_file_size == file_size)
            {
                printf("read src success, file_size=[%d]\n", file_size);
            }
            else
            {
                printf("current read_file_size=[%d], ret=[%d]\n", read_file_size, ret);
                err = -1;
            }
            break;
        }
    }
    if (err < 0)
    {
        goto src_fd_read_err;
    }

    // write packet head to dest file
    int write_file_size = 0;
    struct upgrade_packet_head_desc packet_head =
    {
        .head_flag = PACKET_HEAD_FLAG,
        .version = PACKET_VERSION,
        .file_size = file_size,
        .file_checksum = file_checksum,
    };
    printf("file_size=[%08x], file_checksum=[%08x]\n", file_size, file_checksum);
    uint8_t packet_head_buf[UPGRADE_PACKET_HEAD_SIZE] = {0x0};
    memcpy(packet_head_buf, &packet_head, sizeof(packet_head_buf));
    lseek(dest_fd, 0, SEEK_SET);
    write_file_size = write(dest_fd, packet_head_buf, sizeof(packet_head_buf));
    if (sizeof(packet_head_buf) != write_file_size)
    {
        printf("write packet head fail???\n");
        goto dest_fd_write_err;
    }

    // read src file write to dest file
    lseek(src_fd, 0, SEEK_SET);
    read_file_size = 0;
    for (;;)
    {
        static uint8_t rw_buf[4096] = {0x0};
        int read_ret = read(src_fd, rw_buf, sizeof(rw_buf));
        if (read_ret > 0)
        {
            read_file_size += read_ret;
            int write_ret = write(dest_fd, rw_buf, read_ret);
            if (write_ret == read_ret)
            {
                write_file_size += write_ret;
                continue;
            }
            else
            {
                printf("write fail ??? write_ret=[%d], read_ret=[%d]\n",
                        write_ret, read_ret);
                goto rw_err;
            }
        }
        else
        {
            if (read_file_size == file_size)
            {
                printf("rw pass file_size=[%d], write_file_size=[%d]\n",
                        file_size, write_file_size);
            }
            else
            {
                printf("read fail ??? read_file_size=[%d], file_size=[%d]",
                        read_file_size, packet_head.file_size);
                err = -1;
            }
            break;
        }
    }
    if (err < 0)
    {
        goto rw_err;
    }
    if ((file_size + UPGRADE_PACKET_HEAD_SIZE) == write_file_size)
        printf("packet sucess\n");
    else
        printf("packet fail\n");

    return 0;

rw_err:
dest_fd_write_err:
src_fd_read_err:
    close(dest_fd);
dest_fd_open_err:
    close(src_fd);
src_fd_open_err:
    return 0;
}

