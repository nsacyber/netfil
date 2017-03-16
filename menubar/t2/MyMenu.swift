import Foundation
import AppKit

class MyMenu : NSMenu {
    
    var interface : NSMenuItem?
    var socket : NSMenuItem?
    var ip : NSMenuItem?
    var term : NSMenuItem?
    
    
    var settingsWindow : SettingsWindow?
    
    init() {
        super.init(title: "netfil")

        interface = NSMenuItem(title: "Interface filter", action: #selector(self.launchSettings), keyEquivalent: "")
        //interface?.onStateImage = onImage
        //interface?.offStateImage = offImage
        interface?.target = self;
        
        self.addItem(interface!)
        
        self.addItem(NSMenuItem.separator())
        
        socket = NSMenuItem(title: "Socket filter", action: #selector(self.launchSettings), keyEquivalent: "")
        socket?.target = self
        
        self.addItem(socket!)
        
        self.addItem(NSMenuItem.separator())
        
        ip = NSMenuItem(title: "IP filter", action: #selector(self.launchSettings), keyEquivalent: "")
        self.addItem(ip!)
        ip?.target = self
        
        self.addItem(NSMenuItem.separator())
        
        term = NSMenuItem(title: "Quit netfil", action: #selector(self.terminate), keyEquivalent: "")
        term?.target = self
        self.addItem(term!)
    }
    
    func terminate() {
        exit(0)
    }
    
    func launchSettings(_ sender : NSMenuItem) {
        if((settingsWindow == nil)) {
            settingsWindow = SettingsWindow(contentRect:NSRect(x: 0, y: 0, width: 500, height: 200), styleMask: [NSWindowStyleMask.closable, NSWindowStyleMask.titled], backing: NSBackingStoreType.retained, defer: true, screen: NSScreen.main())
            settingsWindow?.title = "netfil"
            settingsWindow?.isReleasedWhenClosed = false
            settingsWindow?.contentViewController = SettingsViewController()
        }
        
        if(sender == interface) {
            (settingsWindow?.contentViewController?.view as! NSTabView).selectTabViewItem(at: 0)
        } else if(sender == socket) {
            (settingsWindow?.contentViewController?.view as! NSTabView).selectTabViewItem(at: 1)
        } else {
            (settingsWindow?.contentViewController?.view as! NSTabView).selectTabViewItem(at: 2)
        }
        
        settingsWindow?.center()
        settingsWindow?.makeKeyAndOrderFront(self)
        settingsWindow?.orderFrontRegardless()
        settingsWindow?.makeMain()
        settingsWindow?.makeKey()
        NSApp.activate(ignoringOtherApps: true)
        
    }
    
    override func validateMenuItem(_ menuItem: NSMenuItem) -> Bool {
        var ret = true
        
        if(menuItem == interface) {
            let status = Interface.shared.isActive
            switch status {
                case .active:
                    interface?.image = #imageLiteral(resourceName: "Status-user-online-icon")
                    interface?.isEnabled = true
                    break
                case .inactive:
                    interface?.image = #imageLiteral(resourceName: "Status-user-busy-icon")
                    interface?.isEnabled = true
                    break
                default:
                    interface?.image = #imageLiteral(resourceName: "Status-user-offline-icon")
                    ret = false
                    break
            }
            
            interface?.image?.size = NSSize(width: 16, height: 16)
            
        } else if (menuItem == socket) {
            let status = Socket.shared.isActive
            switch status {
            case .active:
                socket?.image = #imageLiteral(resourceName: "Status-user-online-icon")
                socket?.isEnabled = true
                break
            case .inactive:
                socket?.image = #imageLiteral(resourceName: "Status-user-busy-icon")
                socket?.isEnabled = true
                break
            default:
                socket?.image = #imageLiteral(resourceName: "Status-user-offline-icon")
                ret = false
                break
            }
            
            socket?.image?.size = NSSize(width: 16, height: 16)
        }else if (menuItem == ip) {
            let status = IpFilter.shared.isActive
            switch status {
            case .active:
                ip?.image = #imageLiteral(resourceName: "Status-user-online-icon")
                ip?.isEnabled = true
                break
            case .inactive:
                ip?.image = #imageLiteral(resourceName: "Status-user-busy-icon")
                ip?.isEnabled = true
                break
            default:
                ip?.image = #imageLiteral(resourceName: "Status-user-offline-icon")
                ret = false
                break
            }
            
            ip?.image?.size = NSSize(width: 16, height: 16)
        }else if(menuItem == term) {
            return ret
        }
        return ret
    }
    
    required init(coder decoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}
