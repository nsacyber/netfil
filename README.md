# netfil

This is a experimental beginner [Network Kernel Extention (NKE)](https://developer.apple.com/library/content/documentation/Darwin/Conceptual/NKEConceptual/intro/intro.html) with a menu bar application. It can filter by process identifier, IP, or network interfaces using a i/o byte limit. 

This project is NOT meant to be run on a production machine - use at your own risk! Tested on macOS 10.12.1

This is a kernel level alternative to [`netman`](https://github.com/iadgov/netman).

The following NKE filters are used: 
* [Socket Filters](https://developer.apple.com/library/content/documentation/Darwin/Conceptual/NKEConceptual/socket_nke/socket_nke.html) - Filter all sockets by process identitifier
* [IP Filters](https://developer.apple.com/library/content/documentation/Darwin/Conceptual/NKEConceptual/ip_filter_nke/ip_filter_nke.html#//apple_ref/doc/uid/TP40001858-CH229-SW1) - Filter IP traffic by address
* [Interface Filters](https://developer.apple.com/library/content/documentation/Darwin/Conceptual/NKEConceptual/interface_filter_nke/interface_filter_nke.html#//apple_ref/doc/uid/TP40001858-CH230-SW1) - Filter all traffic happening on specified interfaces

### Example use cases

* Create a socket filter to ensure an application only uploads X bytes
* Create an interface filter to ensure you don't go over your data limit when tethering
* Create an IP filter to manage your bandwidth usage to an external host

## KEXT

You can load the KEXT with the `./reload.sh iffilter.kext com.company.netfil` command. Note: Loading unsigned kernel extensions requires System Integrity Protection (SIP) to be turn off. 

## System Controls

This NKE uses sysctl to cross boundaries between kernel code and appliactions. 

You can communicate directly with them using the `sysctl` command.
To view most system controls run `sysctl -a net.netfil`

Interface Filter
* `net.netfil.interface.list` `<struct CCArray>`: `size` is the number of interfaces; `names` contains a list of null-terminated network interface names. 
* `net.netfil.interface.ibyte` `<int>`
* `net.netfil.interface.obyte` `<int>`
* `net.netfil.interface.iobyte` `<int>`
* `net.netfil.interface.status` `<int>`: `0` is "off", `1` is "on"

Socket Filter
* `net.netfil.socket.pid` `<int>`
* `net.netfil.socket.ibyte` `<int>`
* `net.netfil.socket.obyte` `<int>`
* `net.netfil.socket.iobyte` `<int>`
* `net.netfil.socket.status` `<int>`: `0` is "off", `1` is "on"

IP Filter
* `net.netfil.ip.addr` `<struct in_addr>` or `<struct in6_addr>`
* `net.netfil.ip.ibyte` `<int>`
* `net.netfil.ip.obyte` `<int>`
* `net.netfil.ip.iobyte` `<int>`
* `net.netfil.ip.status` `<int>`: `0` is "off", `1` is "on"

Note: You cannot set the `net.netfil.ip.addr` or the `net.netfil.interface.list` via commandline. 

To run a fitler, first set its' options then change its' status to `1`. It is always smart to validate your options before you start. 

For `iobyte`, `ibyte`, and `obyte`, a zero value represents unmetered/unlimited (no filtering).

## Menu Bar / GUI Application

You can use the menu bar application to control the KEXT. A green icon with a dot means the filter is "on", a red icon with a line means the filter is "off", a gray icon with a radar-like symobl means the KEXT is probably not loaded or there is an error. Icon are from Oxygen Team.

## Limitations

### KEXT 

* Right now you can only run one socket, interface, or IP filter at a time. 
* Does not add new interfaces to filter (mac policy might be able to help with this?)
* Anyone can change the variables as root permission is not required. This is very dangerous!
* Not signed so have to jump thru some hoops to get this loaded on your machine. 

### GUI

* Application will only filter IPv4 addresses for the IP filter. 
* Application will not be alerted instantly on KEXT changes. 

## Useful Resources

* https://github.com/gdbinit/tcplognke
* https://github.com/ikob/i-Path
* https://github.com/tesseract2048/netfat/
* https://github.com/applesrc/SharedIP/
* https://github.com/williamluke/peerguardian-linux
* http://phrack.org/issues/69/7.html
* http://soundly.me/discovering-source-folder-when-hiding-files-in-osx-kernel-rootkits/
* kernel debugging
** http://lightbulbone.com/2016/10/04/intro-to-macos-kernel-debugging.html
* mac policies
** https://www.synack.com/2015/11/17/monitoring-process-creation-via-the-kernel-part-i/
** https://developer.apple.com/reference/kernel/mpo_ifnet_label_associate_t?language=objc
* Icons from the Oxygen Icon pack under the GNU Lesser General Public License

## License
See [LICENSE](./LICENSE.md).

## Disclaimer
See [DISCLAIMER](./DISCLAIMER.md).