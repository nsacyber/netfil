import Foundation
import Darwin

class IpFilter : NSObject {
    
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
    
    static var shared : IpFilter = IpFilter()
    
    override init() {
        super.init()
    }
    
    dynamic var isActive : activity {
        get {
            var value : CInt = -1;
            var size = -1;
            let ret = sysctlbyname("net.netfil.ip.status", &value, &size, nil, 0)
            print(ret, value, size)
            if(ret != 0) { return .unknown }
            return ret == 0 && value == 1 ? IpFilter.activity.active : IpFilter.activity.inactive
        }
        set {
            
            self.willChangeValue(forKey: "isActive")
            var nVal : CInt = newValue == IpFilter.activity.active ? 1 : 0
            print("trying to set", nVal)
            let _ = sysctlbyname("net.netfil.ip.status", nil, nil, &nVal, MemoryLayout<CInt>.size)
            self.didChangeValue(forKey: "isActive")
        }
    }
    
    var ioByte : CInt {
        get {
            var value : CInt = -2;
            var size = -3;
            let ret = sysctlbyname("net.netfil.ip.iobyte", &value, &size, nil, 0)
            return ret == 0 ? value : -1
        }
        set {
            var nVal : CInt = newValue
            var value : CInt = -2;
            var size = -3;
            let _ = sysctlbyname("net.netfil.ip.iobyte", &value, &size, &nVal, MemoryLayout<CInt>.size)
        }
    }
    
    var iByte : CInt {
        get {
            var value : CInt = -2;
            var size = -3;
            let ret = sysctlbyname("net.netfil.ip.ibyte", &value, &size, nil, 0)
            return ret == 0 ? value : -1
        }
        set {
            var nVal : CInt = newValue
            var value : CInt = -2;
            var size = -3;
            let _ = sysctlbyname("net.netfil.ip.ibyte", &value, &size, &nVal, MemoryLayout<CInt>.size)
        }
    }
    
    var oByte : CInt {
        get {
            var value : CInt = -2;
            var size = -3;
            let ret = sysctlbyname("net.netfil.ip.obyte", &value, &size, nil, 0)
            return ret == 0 ? value : -1
        }
        set {
            var nVal : CInt = newValue
            var value : CInt = -2;
            var size = -3;
            
            let _ = sysctlbyname("net.netfil.ip.obyte", &value, &size, &nVal, MemoryLayout<CInt>.size)
        }
    }
    
    
    var addr : String {
        get {
            //let a = in_addr()
            //inet_addr()
            
            // TODO: this
            
            
            var a : in_addr_t = in_addr_t()
            var size = MemoryLayout.size(ofValue: a)
            let ret = sysctlbyname("net.netfil.ip.addr", &a, &size, nil, 0)
            
            if(ret == 0) {
                var ipAddressString = [CChar](repeating: 0, count:Int(INET_ADDRSTRLEN))
                let _ = inet_ntop(AF_INET, &a, &ipAddressString, socklen_t(INET_ADDRSTRLEN));
                return String(cString: ipAddressString)
            }
            
            return ""
        }
        set {
            print("set", newValue)

            var a = inet_addr(newValue.UTF8CStringBuffer.baseAddress)
            
            if a == __uint32_t.max {
                print("error")
            }
            

            let _ = sysctlbyname("net.netfil.ip.addr", nil, nil, &a, MemoryLayout.size(ofValue: a))
            
        }
    }
    
}
