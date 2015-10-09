#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

char *portname;
char *scriptpath;

int config_parse ();

int config_parse () {
    FILE *config;
    char *buf;
    char * pointer;
    int counter = 0;
    int key_lenght, eq;
    
    
    config = fopen("config.txt","r");
    if(config == 0) {
        printf("Error : config not found.\n");
        return -1;
    }
    
    fseek(config, 0, SEEK_END);
    int size = ftell(config);
    rewind(config);
    buf = (char*)malloc(size*sizeof(char));
    
    for (int i = 0; i < size; ++i) {
        buf[i] = fgetc(config);
    }
    
    //printf("%ld - lenght\n", strlen(buf));
    //printf("%s", buf);
    
//find SCRIPT
    pointer = strstr(buf, "SCRIPT=");
    if (pointer == NULL) {
        puts("Invalid config\n");
        return -1;
    }
    key_lenght = strlen("SCRIPT=");
    counter = 0;
    eq = pointer - buf;
    
    char * endofline = strchr(pointer, '\n');
    
    //printf("%ld - end\n", endofline - buf);
    //printf("%d - eq\n", eq);
    
    scriptpath = (char*)malloc((endofline - pointer - key_lenght)*sizeof(char));
    
    while ( eq + counter < endofline - buf - key_lenght) {
        scriptpath[counter] = buf[eq + counter + strlen("SCRIPT=")];
        counter++;
    }
    scriptpath[counter] = '\0';
    
    //printf("%ld - lenght\n", strlen(scriptpath));
    //printf ("%s -  script path\n",scriptpath);
    
//find PORT
    pointer = strstr(buf, "PORT=");
    if (pointer == NULL) {
        puts("Invalid config\n");
        return -1;
    }
    key_lenght = strlen("PORT=");
    counter = 0;
    eq = pointer - buf;
    
    endofline = strchr(pointer, '\n');
    
    //printf("%ld - end\n", endofline - buf);
    //printf("%d - eq\n", eq);
    
    portname = (char*)malloc((endofline - pointer - key_lenght)*sizeof(char));

    while ( eq + counter < endofline - buf - key_lenght) {
        portname[counter] = buf[eq + counter + strlen("PORT=")];
        counter++;
    }
    portname[counter] = '\0';
    
    //printf("%ld - lenght\n", strlen(portname));
    //printf ("%s -  portname\n",portname);

    
/*
//parsing port name
    if(!fgets(buf, 100, config)) {
        printf("Error : invalid config.\n");
        return -1;
    };
    printf("%ld\n", strchr(buf, '=') - buf + 1);
    printf("%ld\n", strlen(buf));
    
    int eq = strchr(buf, '=') - buf + 1;
    for (int i = eq; i < strlen(buf); ++i) {
        portname[i - eq] = buf[i];
        printf("%c - ",buf[i]);
        printf("%c - ",portname[i - eq]);
        printf("%ld\n", strlen(portname));

    }
    printf("%ld\n", strlen(portname));
    
    
    
    
    
//parsing path of a script
    if(!fgets(buf, 100, config)) {
        printf("Error : invalid config.\n");
        return -1;
    };
    
    eq = strchr(buf, '=') - buf + 1;
    for (int i = eq; i < strlen(buf); ++i) {
        scriptpath[i - eq] = buf[i];
    }
    */
    
    
    free(buf);
    fclose(config);
    return 0;
}

int main () {
    
    if (config_parse() == -1) {
        return -1;
    }
    
    //printf("%s\n", portname);
    
    int port = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (port < 0) {
        printf ("error %d opening %s: %s", errno, portname, strerror (errno));
        return -1;
    }
    
    FILE *script;
    script = fopen(scriptpath, "r");
    if(script == 0) {
        printf("Error : script file not found.\n");
        return -1;
    }
    
    char command[100];
    char response[100];
    char buf[100];
    
    while (1) {
        if (!fgets(command, 100, script) && !fgets(response, 100, script)) {
            rewind (script);
            sleep(10);
            printf("The programm is stopped for 10 sec\n");
            continue;
        }
        write (port, command, strlen(command));
        read (port, buf, 100);
        if (!strcmp(buf, response)) {
            printf ("Error in response : %s\n", buf);
            break;
        }
    }
    
    close(port);
    free(portname);
    free(scriptpath);
return 0;
}