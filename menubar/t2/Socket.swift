import Foundation

class Socket : NSObject {
    
    @objc
    enum activity : Int, CustomStringConvertible {
        case active
        case inactive
        case unknown
        
        var description : String {
            switch self {
            case .active: return "Active"
            case .inactive: return "Inactive"
            default: return "Unknown"
            }
        }
    }
    
    static var shared : Socket = Socket()
    
    override init() {
        super.init()
    }
    
    dynamic var isActive : activity {
        get {
            var value : CInt = -1;
            var size = -1;
            let ret = sysctlbyname("net.netfil.socket.status", &value, &size, nil, 0)
            print(ret, value, size)
            if(ret != 0) { return .unknown }
            return ret == 0 && value == 1 ? Socket.activity.active : Socket.activity.inactive
        }
        set {
            self.willChangeValue(forKey: "isActive")
            var nVal : CInt = newValue == Socket.activity.active ? 1 : 0
            let _ = sysctlbyname("net.netfil.socket.status", nil, nil, &nVal, MemoryLayout<CInt>.size)
            self.didChangeValue(forKey: "isActive")
        }
    }
    
    var ioByte : CInt {
        get {
            var value : CInt = -2;
            var size = -3;
            let ret = sysctlbyname("net.netfil.socket.iobyte", &value, &size, nil, 0)
            return ret == 0 ? value : -1
        }
        set {
            var nVal : CInt = newValue
            var value : CInt = -2;
            var size = -3;
            let _ = sysctlbyname("net.netfil.socket.iobyte", &value, &size, &nVal, MemoryLayout<CInt>.size)
        }
    }
    
    var iByte : CInt {
        get {
            var value : CInt = -2;
            var size = -3;
            let ret = sysctlbyname("net.netfil.socket.ibyte", &value, &size, nil, 0)
            return ret == 0 ? value : -1
        }
        set {
            var nVal : CInt = newValue
            var value : CInt = -2;
            var size = -3;
            let _ = sysctlbyname("net.netfil.socket.ibyte", &value, &size, &nVal, MemoryLayout<CInt>.size)
        }
    }
    
    var oByte : CInt {
        get {
            var value : CInt = -2;
            var size = -3;
            let ret = sysctlbyname("net.netfil.socket.obyte", &value, &size, nil, 0)
            return ret == 0 ? value : -1
        }
        set {
            var nVal : CInt = newValue
            var value : CInt = -2;
            var size = -3;
            
            let _ = sysctlbyname("net.netfil.socket.obyte", &value, &size, &nVal, MemoryLayout<CInt>.size)
        }
    }
    
    
    var pid : CInt {
        get {
            var value : CInt = -2;
            var size = -3;
            let ret = sysctlbyname("net.netfil.socket.pid", &value, &size, nil, 0)
            return ret == 0 ? value : -1
        }
        set {
            var nVal : CInt = newValue
            var value : CInt = -2;
            var size = -3;
            
            let _ = sysctlbyname("net.netfil.socket.pid", &value, &size, &nVal, MemoryLayout<CInt>.size)
        }
    }
    
}
