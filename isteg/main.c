#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include "bitmap.h"
#include "stego.h"


#define FFMPEG "ffmpeg -t 00:00:15 -i "
#define OUTPUT_IMG "bitmaps/img%05d.bmp"
#define SOURCE_VIDEO "source_video.avi -r 24/1 "
#define MAX_NAME 100

FILE *openforwrite(int filecounter)
{
    char fileoutputname[MAX_NAME];

    sprintf(fileoutputname, "text_files/file_part%d.txt", filecounter);
    return fopen(fileoutputname, "w");
}

int file_count(char path[])
{
	int file_count = 0;
	DIR * dirp;
	struct dirent * entry;

	dirp = opendir(path); /* There should be error handling after this */
	while ((entry = readdir(dirp)) != NULL) {
    	if (entry->d_type == DT_REG) { /* If the entry is a regular file */
         file_count++;
    	}
	
	}
	closedir(dirp);
	return file_count;
}

void ffmpeg_stitch(char out_vid[])
{    
     #define OUT_BMP_PATH "out_bitmaps/img%05d.bmp"
     char out_vid_cmd[100];
     sprintf(out_vid_cmd, "ffmpeg -i %s -vcodec rawvideo %s", OUT_BMP_PATH, out_vid);
     system(out_vid_cmd);
    //system("ffmpeg -i out_bitmaps/img%05d.bmp -vcodec rawvideo outvid.avi");
}

void print_help(char *path)
{
    printf("*** Image Steganography by LSB substitution ***\n\n"
           "Usage:  \n"
           "Run from isteg folder: ./build/isteg\n"
           "%s [-e] <text file to encode> <source video> <destination video>\n"
           "%s [-d] <encoded video> <decoded file>\n\n"
           "-e : Encode text in video\n"
           "-d : Decode text from video\n",
           path, path);
}
/*******Splits text file into many smaller text files*******/
int text_splitter(char text_file_name[])
{
    FILE *ptr_readfile;
    FILE *ptr_writefile;
    char line[300]; 
    int filecounter = 1, linecounter = 1;

    ptr_readfile = fopen(text_file_name, "r");
    if (!ptr_readfile)
        return 1;

    ptr_writefile = openforwrite(filecounter);

    while (fgets(line, sizeof line, ptr_readfile) != NULL)
    {
        if (filecounter < file_count("bitmaps") && linecounter == 7544/file_count("bitmaps"))
        {
            fclose(ptr_writefile);
            linecounter = 1;
            filecounter++;
            ptr_writefile = openforwrite(filecounter);
            if (!ptr_writefile)
                return 1;
        }
        fprintf(ptr_writefile, "%s\n", line);
        linecounter++;
    }
    fclose(ptr_readfile);
}
/*******Merges all the text files into a single file*******/
void text_stitch(char file_out[])
{
    int ch;

    file_out[strcspn(file_out, "\n")] = '\0';

    FILE *fout = fopen(file_out, "w");
    if (!fout)
    {
        perror("fopen output file");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < 316; i++)
    {
        char file_in[MAX_NAME];
        sprintf(file_in, "decoded_messages/file_part%d.txt", i);

        FILE *fin = fopen(file_in, "r");
        if (!fin)
        {
            perror("fopen input file");
            fprintf(stderr, "Problem file: %s\n", file_in);
            exit(EXIT_FAILURE);
        }

        while ((ch = fgetc(fin)) != EOF)
            fputc(ch, fout);

        fclose(fin);
    }

    fclose(fout);
}

int main(int argc, char **argv)
{


    char command[500];

    if (argc != 5 && argc != 4)
    {
        print_help(argv[0]);
        exit(1);
    }
   
    sprintf(command, "%s%s %s", FFMPEG, argv[3], OUTPUT_IMG);
    int mode;
    int i;

    if (!strcmp(argv[1], "-e"))
        mode = 1;
    else if (!strcmp(argv[1], "-d"))
        mode = 0;
    else
    {
        print_help(argv[0]);
        exit(1);
    }

    

    if (mode)
    {
        system("mkdir bitmaps");
        system("mkdir out_bitmaps");       
        system("mkdir text_files"); 


        char bitmaps[MAX_NAME];
        char out_bitmaps[MAX_NAME];
        char fileoutputname[MAX_NAME];

        
        system(command);
        text_splitter(argv[2]);

        for (i = 1; i < file_count("bitmaps")+1; i++)
        {

            sprintf(bitmaps, "bitmaps/img%05d.bmp", i);
            sprintf(fileoutputname, "text_files/file_part%d.txt", i);
            sprintf(out_bitmaps, "out_bitmaps/img%05d.bmp", i);
            
            encode(fileoutputname, bitmaps, out_bitmaps);
            
            printf("encoded image %d\n", i);
            
        }

        ffmpeg_stitch(argv[4]);
        system("rm -r bitmaps");
        system("rm -r text_files");
        system("rm -r out_bitmaps");
        
    }
    else
    {
        #define BITMAP2_PATH "out_bitmaps_2/img%05d.bmp"

        char out_bitmaps[MAX_NAME];
        char fileoutputname[MAX_NAME];
        char split_cmd [500];

        system("mkdir decoded_messages");
        system("mkdir out_bitmaps_2");

        sprintf(split_cmd,"ffmpeg -i %s %s", argv[2], BITMAP2_PATH);
        system(split_cmd);
        for (i = 1; i < file_count("out_bitmaps_2")+1; i++){

            sprintf(out_bitmaps, "out_bitmaps_2/img%05d.bmp", i);
            sprintf(fileoutputname, "decoded_messages/file_part%d.txt", i); 
            decode(out_bitmaps, fileoutputname);
        }
        text_stitch(argv[3]);
        system("rm -r decoded_messages");
        system("rm -r out_bitmaps_2");
    }
     
    
    return EXIT_SUCCESS;
}
