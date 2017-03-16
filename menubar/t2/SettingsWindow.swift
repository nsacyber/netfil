import Foundation
import AppKit

let PADDING : CGFloat = 15

class SettingsWindow : NSWindow {
    
}

class SettingsViewController : NSViewController {
    
    override func loadView() {
        view = NSTabView()
        view.frame.size.width = 500
        //self.preferredContentSize = (view as! NSTabView).minimumSize
        //(view as! NSStackView).orientation = NSUserInterfaceLayoutOrientation.vertical
        //(view as! NSStackView).spacing = CGFloat(PADDING)
        //let titleBarSize = NSWindow.frameRect(forContentRect: self.view.frame, styleMask: NSWindowStyleMask.titled).height
        //(view as! NSStackView).edgeInsets = EdgeInsets(top: titleBarSize + CGFloat(PADDING), left: PADDING, bottom: PADDING, right: PADDING)
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        let one = NSTabViewItem(viewController: InterfaceViewController())
        one.label = "Interface Filter"
        (view as! NSTabView).addTabViewItem(one)
        
        let two = NSTabViewItem(viewController: SocketViewController())
        two.label = "Socket Filter"
        (view as! NSTabView).addTabViewItem(two)
        
        let three = NSTabViewItem(viewController: IpViewController())
        three.label = "IP Filter"
        (view as! NSTabView).addTabViewItem(three)
        
        //(view as! NSStackView).addArrangedSubview(t)
    }
    
}
