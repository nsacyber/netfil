import Foundation
import Cocoa

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

    @IBOutlet weak var window: NSWindow!
    let statusItem = NSStatusBar.system().statusItem(withLength: NSVariableStatusItemLength)

    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application
        
        self.statusItem.title = "netfil"
        self.statusItem.menu = MyMenu()
      
//        print(Interface.isActive, Interface.ioByte, Interface.iByte, Interface.oByte)
//        Interface.list = ["what"]
//        print(Interface.list)
        
        IpFilter.shared.addr = "127.0.0.1"
        print("is set to...", IpFilter.shared.addr)
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }


}

