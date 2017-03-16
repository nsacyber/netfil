import Foundation
import AppKit

class IpViewController : NSViewController {
    
    var button : NSButton?
    
    var addr : NSTextField?
    
    var iByte : NSTextField?
    var oByte : NSTextField?
    var ioByte : NSTextField?
    
    
    override func loadView() {
        view = NSStackView(frame: NSRect(x: 0, y: 0, width: 500, height: 0))
        (view as! NSStackView).orientation = NSUserInterfaceLayoutOrientation.vertical
        (view as! NSStackView).spacing = CGFloat(PADDING)
        let titleBarSize = NSWindow.frameRect(forContentRect: self.view.frame, styleMask: NSWindowStyleMask.titled).height
        (view as! NSStackView).edgeInsets = EdgeInsets(top: titleBarSize, left: PADDING, bottom: PADDING, right: PADDING)
        //(view as! NSStackView).translatesAutoresizingMaskIntoConstraints = false
    }
    
    override func observeValue(forKeyPath keyPath: String?, of object: Any?, change: [NSKeyValueChangeKey : Any]?, context: UnsafeMutableRawPointer?) {
        
        //super.observeValue(forKeyPath: keyPath, of: object, change: change, context: context)
        if(keyPath == "isActive") {
            statusChange()
            print("\(keyPath) did change here!", IpFilter.shared.isActive)
        }
    }
    
    override func viewDidLoad() {
        
        button = NSButton(title: "Go!", target: self, action: #selector(buttonPress))
        
        let iByteName = NSTextField(string: "iByte:")
        iByteName.isBordered = false
        iByteName.isEditable = false
        iByteName.isSelectable = false
        iByteName.backgroundColor = NSColor.clear
        
        let iByteLabel = NSTextField(string: "bytes")
        iByteLabel.isBordered = false
        iByteLabel.isEditable = false
        iByteLabel.isSelectable = false
        iByteLabel.alphaValue = 0.7
        iByteLabel.backgroundColor = NSColor.clear
        
        iByte = NSTextField(string: String(IpFilter.shared.iByte))
        iByte?.placeholderString = "0"
        iByte?.alignment = NSTextAlignment.right
        
        let iByteStack = NSStackView(views: [iByteName, iByte!, iByteLabel])
        iByteStack.orientation = NSUserInterfaceLayoutOrientation.horizontal
        
        let oByteName = NSTextField(string: "oByte:")
        oByteName.isBordered = false
        oByteName.isEditable = false
        oByteName.isSelectable = false
        oByteName.backgroundColor = NSColor.clear
        
        let oByteLabel = NSTextField(string: "bytes")
        oByteLabel.isBordered = false
        oByteLabel.isEditable = false
        oByteLabel.isSelectable = false
        oByteLabel.alphaValue = 0.7
        oByteLabel.backgroundColor = NSColor.clear
        
        oByte = NSTextField(string: String(IpFilter.shared.oByte))
        oByte?.placeholderString = "0"
        oByte?.alignment = NSTextAlignment.right
        
        let oByteStack = NSStackView(views: [oByteName, oByte!, oByteLabel])
        oByteStack.orientation = NSUserInterfaceLayoutOrientation.horizontal
        
        let ioByteName = NSTextField(string: "ioByte:")
        ioByteName.isBordered = false
        ioByteName.isEditable = false
        ioByteName.isSelectable = false
        ioByteName.backgroundColor = NSColor.clear
        
        let ioByteLabel = NSTextField(string: "bytes")
        ioByteLabel.isBordered = false
        ioByteLabel.isEditable = false
        ioByteLabel.isSelectable = false
        ioByteLabel.wantsLayer = true // to enable transparency
        ioByteLabel.alphaValue = 0.7
        ioByteLabel.backgroundColor = NSColor.clear
        
        ioByte = TSTextField(string: String(IpFilter.shared.ioByte))
        ioByte?.placeholderString = "0"
        ioByte?.alignment = NSTextAlignment.right
        
        let ioByteStack = NSStackView(views: [ioByteName, ioByte!, ioByteLabel])
        ioByteStack.orientation = NSUserInterfaceLayoutOrientation.horizontal
        //ioByteStack.alignment = NSLayoutAttribute.right
        
        let addrName = NSTextField(string: "Address:")
        addrName.isBordered = false
        addrName.isEditable = false
        addrName.isSelectable = false
        addrName.backgroundColor = NSColor.clear
        
        addr = TSTextField()
        addr?.alignment = NSTextAlignment.right
        
        let addrStack = NSStackView(views: [addrName, addr!])
        addrStack.orientation = NSUserInterfaceLayoutOrientation.horizontal
        
        //let parentStackView = NSStackView(views: [titleStackView, iByteStack, oByteStack, ioByteStack, mainStackView!, button!])
        //parentStackView.orientation = NSUserInterfaceLayoutOrientation.vertical
        //parentStackView.frame.size.height = 20
        //parentStackView.frame.size.width = 200;
        //self.view = parentStackView
        (view as! NSStackView).addArrangedSubview(iByteStack)
        (view as! NSStackView).addArrangedSubview(oByteStack)
        (view as! NSStackView).addArrangedSubview(ioByteStack)
        (view as! NSStackView).addArrangedSubview(addrStack)
        (view as! NSStackView).addArrangedSubview(button!)
        
        
        
        IpFilter.shared.addObserver(self, forKeyPath: "isActive", options: [NSKeyValueObservingOptions.initial, NSKeyValueObservingOptions.new], context: nil)
        
        Timer.scheduledTimer(timeInterval: 0.2, target: self, selector: #selector(checkStatus), userInfo: nil, repeats: true)
    }
    
    func buttonPress() {
        switch(IpFilter.shared.isActive) {
        case .active:
            IpFilter.shared.isActive = IpFilter.activity.inactive
            break
        case .inactive:
            IpFilter.shared.addr = (addr?.stringValue)!
            IpFilter.shared.iByte = CInt(Int((iByte?.stringValue)!)!)
            IpFilter.shared.oByte = CInt(Int((oByte?.stringValue)!)!)
            IpFilter.shared.ioByte = CInt(Int((ioByte?.stringValue)!)!)
            IpFilter.shared.isActive = IpFilter.activity.active
            break
        default:
            break
        }
        statusChange()
    }
    
    func statusChange() {
        let s = IpFilter.shared.isActive
        lastStatus = s
        switch(IpFilter.shared.isActive) {
        case .active:
            button?.title = "Stop"
            button?.isEnabled = true
            break
        case .inactive:
            button?.title = "Filter"
            button?.isEnabled = true
            break
        default:
            button?.title = "Filter"
            button?.isEnabled = false
        }
    }
    
    var lastStatus : IpFilter.activity = IpFilter.activity.unknown
    func checkStatus() {
        
        // This is necessary cause our KVO only detects changes
        // that we make, not that the kernel makes, or other
        // external programs
        
        if IpFilter.shared.isActive != lastStatus {
            statusChange()
        }
    }
    
}
