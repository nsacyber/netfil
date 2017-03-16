#ifndef common_h
#define common_h

// https://developer.apple.com/library/content/documentation/Darwin/Conceptual/NKEConceptual/reference/reference.html#//apple_ref/doc/uid/TP40001858-CH232-SW1

#include <libkern/libkern.h>
#include <sys/systm.h>
#include <sys/time.h>
#include <sys/proc.h>
#include <sys/malloc.h>
#include <sys/kern_event.h>
#include <mach/mach_types.h>
#include <sys/kernel_types.h>
#include <sys/kpi_mbuf.h> //  includes functions for manipulating mbuf data structures. These are used heavily for passing packets and packet fragments around throughout the protocol stack.
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <net/if_ether.h>
#include <net/kpi_interface.h> // includes functions for manipulating network interfaces, including packet injection, attaching/detaching protocols, attaching/detaching interfaces, and so on.
#include <libkern/OSMalloc.h>
#include <sys/mbuf.h>
#include <sys/param.h>
#include <sys/kauth.h>
#include <sys/sysctl.h>
#include "errors.h"

struct array {
    int size;
    void **values;
};

extern OSMallocTag globalOSMallocTag;

#if defined(DEBUG) && DEBUG > 0
#define DPRINT(fmt, args...) printf("DEBUG: %s:%d:%s(): " fmt, \
__FILE__, __LINE__, __func__, ##args)
#else
#define DPRINT(fmt, args...) /* Don't do anything in release builds */
#endif

#endif /* common_h */
