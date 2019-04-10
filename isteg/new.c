#include <stdio.h>
#include <stdlib.h>

int main(){
    //system("ffmpeg -i source_video.avi bitmaps/yo%04d.bmp");
    system("ffmpeg -i out_bitmaps/img%05d.bmp -r 60 video_out.avi");
    return 0;

}
                
