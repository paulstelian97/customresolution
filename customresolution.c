#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <stdint.h>
typedef enum _bool {false, true} bool;
bool nextissmall=false;
char masterx[6], slavex[6], mastery[6], slavey[6];
char cached_scaling[16]; /*strlen("1.00000x1.00000")+1*/
void readconfig(bool chain);
void setres()
{
    #define commandstring_length 105
    /*strlen("xrandr --output LVDS1 --mode 10000x10000"
    " --scale 1.00000x1.00000 --fb 100000x10000 --panning 10000x10000")+1;*/
    char commandstring[commandstring_length];
    sprintf(commandstring, "xrandr --output LVDS1 --mode %sx%s"
        " --scale %s --fb %sx%s --panning %sx%s",
        masterx, mastery, nextissmall?cached_scaling:"1x1", slavex,
        slavey, slavex, slavey);
    system(commandstring);
    #undef commandstring_length
}
void server_sig_handler(int no)
{
    if(no == SIGUSR2)
    {
        readconfig(true);
        return;
    }
    if(no != SIGUSR1) return;
    setres();
    nextissmall = !nextissmall;
}
void nullhandler(int i){};
void readconfig(bool chain)
{
    int cached_masterx, cached_mastery, cached_slavex, cached_slavey;
    if (fprintf(stderr, "%i\n", !system("[ -e ~/rescfg ]")), !system("[ -e ~/rescfg ]"))
    {
        FILE* f = fopen("/home/paul/rescfg", "r");
        if(!f) perror("Cannot open rescfg");
        if(!f) exit(1);
        fscanf(f, "%i %i %i %i", &cached_masterx, &cached_mastery, &cached_slavex, &cached_slavey);
    }
    else
    {
        strcpy(masterx, "1366");
        strcpy(mastery, "768");
        strcpy(slavex, "2049");
        strcpy(slavey, "1152");
    }

#define SET(x) sprintf(x, "%i", cached_ ## x)
    SET(masterx);
    SET(mastery);
    SET(slavex);
    SET(slavey);
#undef SET
    float scalex = (float)cached_slavex/cached_masterx; /* I don't get why it's*/
    float scaley = (float)cached_slavey/cached_mastery; /* slave/master */
    sprintf(cached_scaling, "%.5fx%.5f", scalex, scaley);
    nextissmall=1;
    if(chain)
    setres();
}
int main(int argc, char **argv)
{
    signal(SIGUSR1, nullhandler);
    signal(SIGUSR2, nullhandler);
    if(argc>1 && strcasecmp("--switch", argv[1])==0){
        system("killall -s SIGUSR1 -- --resdaemon");}
    else
    if(argc>1 && strcasecmp("--reload", argv[1])==0){
        system("killall -s SIGUSR2 -- --resdaemon");}
    else
    {
        printf("ARGUMENTS:\n");
        {
            int array = 0;
            while(array < argc) printf("%i: %s\n", array, argv[array++]);
        }
        if(argc >= 1 && strcasecmp("--resdaemon", argv[0]))
        {system("killall -- --resdaemon");
        execl("/home/paul/customresolution", "--resdaemon", (void*)0);}
        prctl(PR_SET_NAME, (unsigned long)"--resdaemon", 0, 0, 0);
        readconfig(false);
        signal(SIGUSR1, server_sig_handler);
        signal(SIGUSR2, server_sig_handler);
        server_sig_handler(SIGUSR1);
        while(1) sleep(1);
    }
    return 0;
}
