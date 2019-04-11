//
// Created by Miguel Alonso Jr on 2019-02-28.
//

#include "stego.h"
#include "bitmap.h"
#include "util.h"


unsigned char _get_bit(char byte, int bit_number) {
    return (unsigned char) ((byte >> bit_number) & 1);
}

void _store_length(long size, unsigned char *data) {
    data[0] = (unsigned char) ((size >> 24) & 0xFF);
    data[1] = (unsigned char) ((size >> 16) & 0xFF);
    data[2] = (unsigned char) ((size >> 8) & 0xFF);
    data[3] = (unsigned char) (size & 0xFF);
}

long _get_length(const unsigned char *data) {
    long length = 0;
    length = (long) (data[3] & 0xFF);
    length = length | (long) ((data[2] & 0xFF) << 8);
    length = length | (long) ((data[1] & 0xFF) << 16);
    length = length | (long) ((data[0] & 0xFF) << 24);

    return length;
}

bool encode(char *text_source, char *original_image, char *destination_image) {
    //TODO: add size of text source to encoded image
    char *error_message = NULL;
    char buff;
    int index = 4;
    unsigned char mask = 1;
    FILE *text_file = _open_file(text_source, "r");
    long text_size = _get_text_length(text_file);
    Bitmap *image = read_bitmap(original_image, &error_message);
    _store_length(text_size, image->data);
    do {
        buff = (char) fgetc(text_file);
        for (int i = 0; i < 8; i++) {
            image->data[index] = (image->data[index] & ~mask) | (_get_bit(buff, i) & mask);
            index++;
        }
    } while (!feof(text_file));

    write_bitmap(destination_image, image, &error_message);
    bool is_valid = check_bitmap_header(image->header, "sw_poster_copy.bmp");
    return is_valid;
}

bool decode(char *image_source, char *text_destination) {
    char *error_message = NULL;
    unsigned char buff = 0;
    unsigned char mask = 1;
    long message_length;
    Bitmap *image = read_bitmap(image_source, &error_message);
    FILE *text_file = _open_file(text_destination, "w");
    message_length = _get_length(image->data);
    for (int index = 4; index < message_length * 8; index++) {
        buff = buff | ((image->data[index] & mask) << ((index - 4) % 8));
        if (((index - 4) % 8) > 6) {
            fputc(buff, text_file);
            buff = 0;
        }
    }
    fclose(text_file);
    bool is_valid = true;
    return is_valid;
}

//TODO: decode

