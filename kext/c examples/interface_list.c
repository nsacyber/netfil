#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

struct array {
    int size;
    void **names;
} array;

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");

    // Set the interface list to 'en4'

    char *name = "net.netfil.interface.list";
    int retval = 0;
    struct array r;
    r.size = 1;
    char *test = "en4";
    r.names = malloc(r.size * sizeof(char*));
    r.names[0] = test;
    size_t len;
    len=sizeof(struct array);
    
    printf("%s %p %p\n", r.names[0], r.names[0], r.names);
    retval=sysctlbyname(name, NULL, NULL, &r, len); // set userInterface
    

    free(r.names);

    if(retval != 0) {
        printf("Unable to set the interface list\n");
        return -1;
    }

    // Now lets confirm that our list was saved by getting the list from 
    // the kernel 


    len = sizeof(int);
    int size = -1;
    
    retval = sysctlbyname(name, &size, &len, NULL, 0); // get count

    if(retval != 0) {
        printf("Unable to get the size of the interface list\n");
        return retval;
    }

    // now that we know how many items are in the list, let's get the 
    // actual list that is in the kernel

    struct array r2; 
    r2.size = size;
    r2.names = malloc(r2.size * sizeof(char *));
    len = sizeof(struct array);
    for(int i = 0; i < r2.size; i++) {
        r2.names[i] = malloc(MAXCOMLEN);
    }

    retval=sysctlbyname(name, &r2, &len, NULL, 0); // get rar

    if(retval != 0) {
        printf("Unable to get the interface list\n");
    }

    for(int i = 0; i < r2.size; i++) {
        free(r2.names[i])
    }

    free(r2.names);

    #ifdef START_IT

    // you can set the byte limits, with zero being no-limit / unlimited

    int n = 0;
    retval = sysctlbyname("net.netfil.interface.ibyte", NULL, NULL, &n, sizeof(n));

    n = 0;
    retval = sysctlbyname("net.netfil.interface.obyte", NULL, NULL, &n, sizeof(n));

    n = 0;
    retval = sysctlbyname("net.netfil.interface.iobyte", NULL, NULL, &n, sizeof(n));

    // then set the status to 1 to start the filter!

    n = 1;
    retval = sysctlbyname("net.netfil.interface.status", NULL, NULL, &n, sizeof(n));

    if(retval != 0) {
        printf("Unable to start the interface filter\n");
    }

    #endif

    return retval;
}
