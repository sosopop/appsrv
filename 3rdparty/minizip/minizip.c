/* minizip.c
   Version 2.5.1, August 18, 2018
   part of the MiniZip project

   Copyright (C) 2010-2018 Nathan Moinvaziri
     https://github.com/nmoinvaz/minizip
   Copyright (C) 1998-2010 Gilles Vollant
     http://www.winimage.com/zLibDll/minizip.html

   This program is distributed under the terms of the same license as zlib.
   See the accompanying LICENSE file for the full text of the license.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <errno.h>

#include "mz.h"
#include "mz_os.h"
#include "mz_strm.h"
#include "mz_strm_buf.h"
#include "mz_strm_split.h"
#include "mz_zip.h"
#include "mz_zip_rw.h"

/***************************************************************************/

void minizip_banner(void)
{
    printf("Minizip %s - https://github.com/nmoinvaz/minizip\n", MZ_VERSION);
    printf("---------------------------------------------------\n");
}

void minizip_help(void)
{
    printf("Usage : minizip [-x -d dir|-l|-e] [-o] [-a] [-j] [-0 to -9] [-b|-m] [-k 512] [-p pwd] [-s] file.zip [files]\n\n" \
           "  -x  Extract files\n" \
           "  -l  List files\n" \
           "  -d  Destination directory\n" \
           "  -o  Overwrite existing files\n" \
           "  -a  Append to existing zip file\n" \
           "  -i  Include full path of files\n" \
           "  -0  Store only\n" \
           "  -1  Compress faster\n" \
           "  -9  Compress better\n" \
           "  -k  Disk size in KB\n" \
           "  -p  Encryption password\n");
#ifdef HAVE_AES
    printf("  -s  AES encryption\n");
#endif
#ifdef HAVE_BZIP2
    printf("  -b  BZIP2 compression\n");
#endif
#ifdef HAVE_LZMA
    printf("  -m  LZMA compression\n");
#endif
    printf("\n");
}

/***************************************************************************/

typedef struct minizip_opt_s {
    uint8_t include_path;
    int16_t compress_level;
    uint8_t compress_method;
    uint8_t overwrite;
    uint8_t append;
    int64_t disk_size;
#ifdef HAVE_AES
    uint8_t aes;
#endif
} minizip_opt;

/***************************************************************************/

int32_t minizip_list(const char *path)
{
    mz_zip_file *file_info = NULL;
    uint32_t ratio = 0;
    int16_t level = 0;
    int32_t err = MZ_OK;
    struct tm tmu_date;
    const char *string_method = NULL;
    char crypt = ' ';

    void *reader = NULL;


    mz_zip_reader_create(&reader);
    err = mz_zip_reader_open_file(reader, path);

    if (err != MZ_OK)
    {
        printf("Error %d opening zip file %s\n", err, path);
        mz_zip_reader_delete(&reader);
        return err;
    }

    err = mz_zip_reader_goto_first_entry(reader);
        
    if (err != MZ_OK && err != MZ_END_OF_LIST)
    {
        printf("Error %d going to first entry in zip file\n", err);
        mz_zip_reader_delete(&reader);
        return err;
    }

    printf("  Length  Method     Size  Attribs Ratio   Date    Time   CRC-32     Name\n");
    printf("  ------  ------     ----  ------- -----   ----    ----   ------     ----\n");

    do
    {
        err = mz_zip_reader_entry_get_info(reader, &file_info);

        if (err != MZ_OK)
        {
            printf("Error %d getting entry info in zip file\n", err);
            break;
        }

        ratio = 0;
        if (file_info->uncompressed_size > 0)
            ratio = (uint32_t)((file_info->compressed_size * 100) / file_info->uncompressed_size);

        // Display a '*' if the file is encrypted
        if (file_info->flag & MZ_ZIP_FLAG_ENCRYPTED)
            crypt = '*';

        switch (file_info->compression_method)
        {
        case MZ_COMPRESS_METHOD_STORE:
            string_method = "Stored";
            break;
        case MZ_COMPRESS_METHOD_DEFLATE:
            level = (int16_t)((file_info->flag & 0x6) / 2);
            if (level == 0)
                string_method = "Defl:N";
            else if (level == 1)
                string_method = "Defl:X";
            else if ((level == 2) || (level == 3))
                string_method = "Defl:F"; // 2: fast , 3: extra fast
            else
                string_method = "Defl:?";
            break;
        case MZ_COMPRESS_METHOD_BZIP2:
            string_method = "BZip2";
            break;
        case MZ_COMPRESS_METHOD_LZMA:
            string_method = "LZMA";
            break;
        default:
            string_method = "Unknwn";
        }

        mz_zip_time_t_to_tm(file_info->modified_date, &tmu_date);

        printf(" %7"PRIu64"  %6s%c %7"PRIu64" %8"PRIx32" %3"PRIu32"%%  %2.2"PRIu32"-%2.2"PRIu32\
               "-%2.2"PRIu32"  %2.2"PRIu32":%2.2"PRIu32"  %8.8"PRIx32"   %s\n",
                file_info->uncompressed_size, string_method, crypt,
                file_info->compressed_size, file_info->external_fa, ratio,
                (uint32_t)tmu_date.tm_mon + 1, (uint32_t)tmu_date.tm_mday,
                (uint32_t)tmu_date.tm_year % 100,
                (uint32_t)tmu_date.tm_hour, (uint32_t)tmu_date.tm_min,
                file_info->crc, file_info->filename);

        err = mz_zip_reader_goto_next_entry(reader);

        if (err != MZ_OK && err != MZ_END_OF_LIST)
        {
            printf("Error %d going to next entry in zip file\n", err);
            break;
        }
    }
    while (err == MZ_OK);

    mz_zip_reader_delete(&reader);

    if (err == MZ_END_OF_LIST)
        return MZ_OK;

    return err;
}

/***************************************************************************/

int32_t minizip_add_entry_cb(void *handle, void *userdata, mz_zip_file *file_info)
{
    printf("Adding %s\n", file_info->filename);
    return MZ_OK;
}

int32_t minizip_add_progress_cb(void *handle, void *userdata, mz_zip_file *file_info, uint64_t position)
{
    double progress = 0;
    uint8_t raw = 0;

    mz_zip_writer_get_raw(handle, &raw);

    if (raw && file_info->compressed_size > 0)
        progress = ((double)position / file_info->compressed_size) * 100;
    else if (!raw && file_info->uncompressed_size > 0)
        progress = ((double)position / file_info->uncompressed_size) * 100;

    printf("%s - %lld / %lld (%.02f%%)\n", file_info->filename, position, file_info->uncompressed_size, progress);
    return MZ_OK;

}
int32_t minizip_add_overwrite_cb(void *handle, void *userdata, const char *path)
{
    minizip_opt *options = (minizip_opt *)userdata;

    if (options->overwrite == 0)
    {
        // If ask the user what to do because append and overwrite args not set
        char rep = 0;
        do
        {
            char answer[128];
            printf("The file %s exists. Overwrite ? [y]es, [n]o, [a]ppend : ", path);
            if (scanf("%1s", answer) != 1)
                exit(EXIT_FAILURE);
            rep = answer[0];

            if ((rep >= 'a') && (rep <= 'z'))
                rep -= 0x20;
        }
        while ((rep != 'Y') && (rep != 'N') && (rep != 'A'));

        if (rep == 'A')
        {
            return MZ_EXIST_ERROR;
        }
        else if (rep == 'N')
        {
            return MZ_INTERNAL_ERROR;
        }
    }

    return MZ_OK;
}

int32_t minizip_add(const char *path, const char *password, minizip_opt *options, int32_t arg_count, const char **args)
{
    void *writer = NULL;
    int32_t err = MZ_OK;
    int32_t err_close = MZ_OK;
    int32_t i = 0;
    const char *filename_in_zip = NULL;


    printf("Archive %s\n", path);

    mz_zip_writer_create(&writer);
    mz_zip_writer_set_password(writer, password);
    mz_zip_writer_set_compress_method(writer, options->compress_method);
    mz_zip_writer_set_compress_level(writer, options->compress_level);
    mz_zip_writer_set_overwrite_cb(writer, options, minizip_add_overwrite_cb);
    mz_zip_writer_set_progress_cb(writer, options, minizip_add_progress_cb);

    err = mz_zip_writer_open_file(writer, path, options->disk_size, options->append);

    if (err == MZ_OK)
    {
        for (i = 0; i < arg_count; i += 1)
        {
            filename_in_zip = args[i];

            err = mz_zip_writer_add_path(writer, filename_in_zip, NULL, options->include_path, 1);
            if (err != MZ_OK)
                printf("Error %d adding path to zip %s\n", err, filename_in_zip);
        }
    }
    else
    {
        printf("Error %d opening zip for writing\n", err);
    }
    
    err_close = mz_zip_writer_close(writer);
    if (err_close != MZ_OK)
    {
        printf("Error %d closing zip for writing %s\n", err_close, path);
        err = err_close;
    }

    mz_zip_writer_delete(&writer);
    return err;
}

/***************************************************************************/

int32_t minizip_extract_entry_cb(void *handle, void *userdata, mz_zip_file *file_info, const char *path)
{
    printf("Extracting %s\n", file_info->filename);
    return MZ_OK;
}

int32_t minizip_extract_progress_cb(void *handle, void *userdata, mz_zip_file *file_info, uint64_t position)
{
    double progress = 0;
    uint8_t raw = 0;
    
    mz_zip_reader_get_raw(handle, &raw);

    if (raw && file_info->compressed_size > 0)
        progress = ((double)position / file_info->compressed_size) * 100;
    else if (!raw && file_info->uncompressed_size > 0)
        progress = ((double)position / file_info->uncompressed_size) * 100;

    printf("%s - %lld / %lld (%.02f%%)\n", file_info->filename, position, file_info->uncompressed_size, progress);
    return MZ_OK;

}
int32_t minizip_extract_overwrite_cb(void *handle, void *userdata, mz_zip_file *file_info, const char *path)
{
    minizip_opt *options = (minizip_opt *)userdata;

    if (options->overwrite == 0)
    {
        char rep = 0;
        do
        {
            char answer[128];
            printf("The file %s exists. Overwrite ? [y]es, [n]o, [A]ll: ", path);
            if (scanf("%1s", answer) != 1)
                exit(EXIT_FAILURE);
            rep = answer[0];
            if ((rep >= 'a') && (rep <= 'z'))
                rep -= 0x20;
        } while ((rep != 'Y') && (rep != 'N') && (rep != 'A'));

        if (rep == 'N')
            return MZ_EXIST_ERROR;
        if (rep == 'A')
            options->overwrite = 1;
    }

    return MZ_OK;
}

int32_t minizip_extract(const char *path, const char *pattern, const char *destination, const char *password, minizip_opt *options)
{
    void *reader = NULL;
    int32_t err = MZ_OK;
    int32_t err_close = MZ_OK;


    printf("Archive %s\n", path);

    mz_zip_reader_create(&reader);
    mz_zip_reader_set_pattern(reader, pattern, 1);
    mz_zip_reader_set_password(reader, password);
    mz_zip_reader_set_entry_cb(reader, options, minizip_extract_entry_cb);
    mz_zip_reader_set_progress_cb(reader, options, minizip_extract_progress_cb);
    mz_zip_reader_set_overwrite_cb(reader, options, minizip_extract_overwrite_cb);

    err = mz_zip_reader_open_file(reader, path);

    if (err != MZ_OK)
    {
        printf("Error %d opening zip file %s\n", err, path);
    }
    else
    {
        err = mz_zip_reader_save_all(reader, destination);
        if (err == MZ_END_OF_LIST && pattern != NULL)
            printf("Files matching %s not found in zip file\n", pattern);
        if (err != MZ_OK)
            printf("Error %d saving zip entries to disk %s\n", err, path);
    }
    err_close = mz_zip_reader_close(reader);
    if (err_close != MZ_OK)
    {
        printf("Error %d closing zip for reading\n", err_close);
        err = err_close;
    }

    mz_zip_reader_delete(&reader);
    return err;
}

/***************************************************************************/

int32_t minizip_erase(const char *src_path, const char *target_path, int32_t arg_count, const char **args)
{
    mz_zip_file *file_info = NULL;
    const char *filename_in_zip = NULL;
    void *reader = NULL;
    void *writer = NULL;
    int32_t err = MZ_OK;
    int32_t i = 0;

    mz_zip_reader_create(&reader);
    mz_zip_writer_create(&writer);

    err = mz_zip_reader_open_file(reader, src_path);
    if (err != MZ_OK)
    {
        printf("Error %d opening zip for reading %s\n", err, src_path);
        mz_zip_reader_delete(&reader);
        return err;
    }

    err = mz_zip_writer_open_file(writer, target_path, 0, 0);
    if (err != MZ_OK)
    {
        printf("Error %d opening zip for writing %s\n", err, target_path);
        mz_zip_reader_delete(&reader);
        mz_zip_writer_delete(&writer);
        return err;
    }

    err = mz_zip_reader_goto_first_entry(reader);

    if (err != MZ_OK && err != MZ_END_OF_LIST)
        printf("Error %d going to first entry in zip file\n", err);

    while (err == MZ_OK)
    {
        err = mz_zip_reader_entry_get_info(reader, &file_info);
        if (err != MZ_OK)
        {
            printf("Error %d getting info from zip\n", err);
            break;
        }

        for (i = 0; i < arg_count; i += 1)
        {
            filename_in_zip = args[i];

            if (mz_path_compare_wc(file_info->filename, filename_in_zip, 1) != MZ_OK)
            {
                printf("Copying %s\n", file_info->filename);
                err = mz_zip_writer_copy_from_reader(writer, reader);
            }
            else
            {
                printf("Skipping %s\n", file_info->filename);
            }
        }

        if (err != MZ_OK)
        {
            printf("Error %d copying entry into new zip\n", err);
            break;
        }

        err = mz_zip_reader_goto_next_entry(reader);

        if (err != MZ_OK && err != MZ_END_OF_LIST)
            printf("Error %d going to next entry in zip file\n", err);
    }

    mz_zip_reader_close(reader);
    mz_zip_reader_delete(&reader);

    mz_zip_writer_close(writer);
    mz_zip_writer_delete(&writer);

    if (err == MZ_END_OF_LIST)
        return MZ_OK;

    return err;
}

/***************************************************************************/

#if !defined(MZ_NO_MAIN)
int main(int argc, const char *argv[])
{
    minizip_opt options;
    void *handle = NULL;
    void *target_handle = NULL;
    void *file_stream = NULL;
    void *split_stream = NULL;
    void *buf_stream = NULL;
    void *tmp_file_stream = NULL;
    void *tmp_split_stream = NULL;
    void *tmp_buf_stream = NULL;
    int64_t disk_size = 0;
    int32_t path_arg = 0;
    int32_t err_close = 0;
    int32_t err = 0;
    int32_t i = 0;
    int16_t mode = 0;
    uint8_t do_list = 0;
    uint8_t do_extract = 0;
    uint8_t do_erase = 0;
    char bak_path[256];
    char tmp_path[256];
    const char *path = NULL;
    const char *password = NULL;
    const char *destination = NULL;
    const char *filename_to_extract = NULL;


    minizip_banner();
    if (argc == 1)
    {
        minizip_help();
        return 0;
    }

    memset(&options, 0, sizeof(options));

    options.compress_method = MZ_COMPRESS_METHOD_DEFLATE;
    options.compress_level = MZ_COMPRESS_LEVEL_DEFAULT;

    // Parse command line options
    for (i = 1; i < argc; i += 1)
    {
        if (argv[i][0] == '-')
        {
            char c = argv[i][1];
            if ((c == 'l') || (c == 'L'))
                do_list = 1;
            if ((c == 'x') || (c == 'X'))
                do_extract = 1;
            if ((c == 'e') || (c == 'E'))
                do_erase = 1;
            if ((c == 'a') || (c == 'A'))
                options.append = 1;
            if ((c == 'o') || (c == 'O'))
                options.overwrite = 1;
            if ((c == 'i') || (c == 'I'))
                options.include_path = 1;
            if ((c >= '0') && (c <= '9'))
            {
                options.compress_level = (c - '0');
                if (options.compress_level == 0)
                    options.compress_method = MZ_COMPRESS_METHOD_STORE;
            }

#ifdef HAVE_BZIP2
            if ((c == 'b') || (c == 'B'))
                options.compress_method = MZ_COMPRESS_METHOD_BZIP2;
#endif
#ifdef HAVE_LZMA
            if ((c == 'm') || (c == 'M'))
                options.compress_method = MZ_COMPRESS_METHOD_LZMA;
#endif
#ifdef HAVE_AES
            if ((c == 's') || (c == 'S'))
                options.aes = 1;
#endif
            if (((c == 'k') || (c == 'K')) && (i + 1 < argc))
            {
                options.disk_size = atoi(argv[i + 1]) * 1024;
                i += 1;
            }
            if (((c == 'd') || (c == 'D')) && (i + 1 < argc))
            {
                destination = argv[i + 1];
                i += 1;
            }
            if (((c == 'p') || (c == 'P')) && (i + 1 < argc))
            {
                password = argv[i + 1];
                i += 1;
            }
        }
        else if (path_arg == 0)
            path_arg = i;
    }

    if (path_arg == 0)
    {
        minizip_help();
        return 0;
    }

    path = argv[path_arg];

    if (do_list)
    {
        err = minizip_list(path);
    }
    else if (do_extract)
    {
        if (argc > path_arg + 1)
            filename_to_extract = argv[path_arg + 1];

        err = minizip_extract(path, filename_to_extract, destination, password, &options);
    }
    else if (do_erase)
    {
        strncpy(tmp_path, path, sizeof(tmp_path));
        strncat(tmp_path, ".tmp", sizeof(tmp_path) - strlen(tmp_path) - 1);

        err = minizip_erase(path, tmp_path, argc - (path_arg + 1), &argv[path_arg + 1]);
    }
    else
    {
        err = minizip_add(path, password, &options, argc - (path_arg + 1), &argv[path_arg + 1]);
    }

    if (err == MZ_OK && do_erase)
    {
        // Swap zip with temporary zip, backup old zip if possible
        strncpy(bak_path, path, sizeof(bak_path));
        strncat(bak_path, ".bak", sizeof(bak_path) - strlen(bak_path) - 1);

        if (mz_os_file_exists(bak_path) == MZ_OK)
            mz_os_delete(bak_path);

        if (mz_os_rename(path, bak_path) != MZ_OK)
            printf("Error backing up zip before replacing %s\n", bak_path);

        if (mz_os_rename(tmp_path, path) != MZ_OK)
            printf("Error replacing zip with temp %s\n", tmp_path);
    }

    return err;
}
#endif