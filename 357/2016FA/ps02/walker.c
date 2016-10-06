#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <limits.h>
#include <errno.h>
#include <dirent.h>
#include <grp.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

int printinfo(struct stat s, char *p){      //given stat and pathname, prints entry in the form you wanted
    printf("%#o/%d  ", s.st_dev, s.st_ino);

    unsigned short mode = s.st_mode;        //print filetype
    if(S_ISDIR(mode))
        printf("d");
    else if(S_ISBLK(mode))
        printf("b");
    else if(S_ISCHR(mode))
        printf("c");
    else if(S_ISFIFO(mode))
        printf("p");
    else if(S_ISLNK(mode))
        printf("l");
    else
        printf("-");

    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    if(mode & S_ISUID)
        printf("s");
    else if(mode & S_IXUSR)
        printf("x");
    else
        printf("-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    if(mode & S_ISGID)
        printf("s");
    else if(mode & S_IXGRP)
        printf("x");
    else
        printf("-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");

    printf(" %d ", s.st_nlink);

    struct passwd *pwent = getpwuid(s.st_uid);
    struct group *g = getgrgid(s.st_gid);
    printf("%8s %8s ", pwent->pw_name, g->gr_name);

    if(S_ISCHR(mode) || S_ISBLK(mode)){
        printf("%#6x ", s.st_rdev);
    }else{
        printf("%6d ", s.st_size);
    }

    struct tm *t = localtime(&s.st_mtime);
    char *c_t = asctime(t);
    c_t[24] = 0;        //why is this implemented this way to add a \n
    printf("%s ", c_t);
    printf("%s", p);

    char linkbuf[PATH_MAX+1];
    if(S_ISLNK(mode)){
        memset(linkbuf, 0, PATH_MAX+1);
        readlink(p, linkbuf, PATH_MAX);  //and why the fuck is this implemented to not add a null terminator
        printf(" -> %s\n", linkbuf);
    }else{
        printf("\n");
    }
}

int walker(char *d, unsigned int u, int m, int x, char *s, int depth){

    if(depth == 1024){
        fprintf(stderr, "Error: You are already too deep! Not going any deeper ;)\n");
        return 0;
    }

    struct stat st;

    //Code to handle if '-l' option was present
    // Finds and saves the device and inode number of target
    unsigned int sl_ino;
    unsigned int sl_dev;
    char linkbuf[PATH_MAX+1];
    if(s){
        realpath(s, linkbuf);
        if(lstat(linkbuf, &st)){
            fprintf(stderr, "Error lstat-ing target(%s). %s\n", linkbuf, strerror(errno));
            exit(-1);
        }
        sl_ino = st.st_ino;
        sl_dev = st.st_dev;
        linkbuf[0] = 0;
    }
    //printf("%d:%d\n", sl_dev, sl_ino);

    char r_path[PATH_MAX+1];
    if(realpath(d, r_path) != r_path){
        fprintf(stderr, "Error expanding real pathname: %s\n", strerror(errno));
        exit(-1);
    }
    
    DIR *dp = opendir(r_path);
    if (dp == NULL) {
        fprintf(stderr, "Error opening directory: %s\n", strerror(errno));
        exit(-1);
    }
    
    char ent_path[PATH_MAX+1];
    struct dirent *ent;

    if(lstat(r_path, &st)){
        fprintf(stderr, "Error lstat-ing directory %s: %s\n", d, strerror(errno));
        exit(-1);
    }
    unsigned int dev_id = st.st_dev;

    //used for second stat's that needs to be done without overwriting st
    struct stat fuck; //!

    while(ent = readdir(dp)){
        ent_path[0] = 0;
        linkbuf[0] = 0;
        strcat(ent_path, r_path);
        strcat(ent_path, "/");
        strcat(ent_path, ent->d_name);

        if(!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")){
            continue;
        }

        if(lstat(ent_path, &st)) fprintf(stderr, "lstat(2) failed on %s\n", ent_path); //Error but continue listing

        unsigned short mod = st.st_mode; //this line is no longer necessary with the introduction of the 'fuck' variable
        //by "line no longer necessary," I mean that I don't have to store st.st_mode anymore because it is not being changed

        //handles '-u' option
        if((u != -1) && (u != st.st_uid)){
            continue;
        }

        /*
            I am aware that there's a better way to implement the handling of the options by
            passing the arguments to the print function and putting the comparison code there.
            That method does not need to call lstat multiple times per file.
        */

        //handles '-l' option
        if(s){
            if(!S_ISLNK(mod)) continue;

            memset(linkbuf, 0, PATH_MAX+1);
            readlink(ent_path, linkbuf, PATH_MAX);
            //printf("%s\n", linkbuf);
            lstat(linkbuf, &fuck);
            //printf("%d:%d:%s\n", fuck.st_ino, fuck.st_dev, ent_path);
            if(fuck.st_ino != sl_ino || fuck.st_dev != sl_dev){
                continue;
            }
        }

        //handles '-m' option
        if(m){
            time_t now = time(NULL);
            if((m < 0) && (now-st.st_mtime > -1*m)){
                continue;
            }
            if(now-st.st_mtime < m) continue;
        }

        //print entry
        printinfo(st, ent_path);

        //descend
        if(S_ISDIR(mod)){
            //printf(ent_path);
            lstat(ent_path, &st);
            if(!x || dev_id == st.st_dev)
                walker(ent_path, u, m, x, s, depth+1);
            else
                fprintf(stderr, "Error: -x option and %s is on a different volume\n", ent_path);
        }
    }

    closedir(dp);
    return 0;
}

int is_num(char *str){
    if(*str == '-'){
        str++;
    }
        
    while(*str){
        if(*str < '0' || *str > '9')
            return 0;
        str++;
    }
    return 1;
}

int main(int argc, char **argv){

    // Parameters
    //  uid_filter has either uid number or -1 if not supplied
    //  sym_target has string pointer or NULL if not supplied
    //  xmnt is either 1 or 0 if not supplied
    //  mtime has either mtime value or 0 if not supplied
    unsigned int uid_filter = -1;
    char *sym_target = NULL;
    int xmnt = 0;
    int mtime = 0;

    //parse options
    char opt;
    while((opt = getopt(argc, argv, "u:m:xl:")) != -1){
        if(opt == 'u'){
            if(is_num(optarg)){
                uid_filter = atoi(optarg);
                if(*optarg == '-'){
                    fprintf(stderr, "Error: invalid uid value %d\n", uid_filter);
                    exit(-1);
                }
            }else{
                struct passwd *ent = getpwnam(optarg);
                if(ent)
                    uid_filter = ent -> pw_uid;
                else{
                    fprintf(stderr, "Error: user %s does not exist!\n", optarg);
                    exit(-1);
                }
            }
        }else if(opt == 'm'){
            if(is_num(optarg))
                mtime = atoi(optarg);
            else{
                fprintf(stderr, "Error: invalid '-m' argument: %s\n", optarg);
                exit(-1);
            }
        }else if(opt == 'x'){
            xmnt = 1;
        }else if(opt == 'l'){
            sym_target = optarg;
        }else{
            exit(-1);
        }
    }

    char *s_path = argv[optind];
    if(!s_path){
        fprintf(stderr, "No starting path provided!\nUsage: %s [options] <starting_path>\n", argv[0]);
        exit(-1);
    }

/*
    printf("Filter uid: %d\n", uid_filter);
    printf("Target: %s\n", sym_target ? sym_target : "" );
    printf("Cross mountpoints? %d\n", xmnt);
    printf("mtime: %d\n", mtime);
    printf("Others: %s\n", argv[optind]);
    printf("----------------------------------------------\n");
*/

    walker(s_path, uid_filter, mtime, xmnt, sym_target, 1);
}
