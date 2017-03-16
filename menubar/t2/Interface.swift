import Foundation

class Interface : NSObject {
    
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
    
    static var shared : Interface = Interface()
    
    override init() {
        super.init()
    }
    
    dynamic var isActive : activity {
        get {
            var value : CInt = -1;
            var size = -1;
            let ret = sysctlbyname("net.netfil.interface.status", &value, &size, nil, 0)
            print(ret, value, size)
            if(ret != 0) { return .unknown }
            return ret == 0 && value == 1 ? Interface.activity.active : Interface.activity.inactive
        }
        set {
            self.willChangeValue(forKey: "isActive")
            var nVal : CInt = newValue == Interface.activity.active ? 1 : 0
            let _ = sysctlbyname("net.netfil.interface.status", nil, nil, &nVal, MemoryLayout<CInt>.size)
            self.didChangeValue(forKey: "isActive")
        }
    }
    
    var ioByte : CInt {
        get {
            var value : CInt = -2;
            var size = -3;
            let ret = sysctlbyname("net.netfil.interface.iobyte", &value, &size, nil, 0)
            return ret == 0 ? value : -1
        }
        set {
            var nVal : CInt = newValue
            var value : CInt = -2;
            var size = -3;
            let _ = sysctlbyname("net.netfil.interface.iobyte", &value, &size, &nVal, MemoryLayout<CInt>.size)
        }
    }
    
    var iByte : CInt {
        get {
            var value : CInt = -2;
            var size = -3;
            let ret = sysctlbyname("net.netfil.interface.ibyte", &value, &size, nil, 0)
            return ret == 0 ? value : -1
        }
        set {
            var nVal : CInt = newValue
            var value : CInt = -2;
            var size = -3;
            let _ = sysctlbyname("net.netfil.interface.ibyte", &value, &size, &nVal, MemoryLayout<CInt>.size)
        }
    }
    
    var oByte : CInt {
        get {
            var value : CInt = -2;
            var size = -3;
            let ret = sysctlbyname("net.netfil.interface.obyte", &value, &size, nil, 0)
            return ret == 0 ? value : -1
        }
        set {
            var nVal : CInt = newValue
            var value : CInt = -2;
            var size = -3;
            
            let _ = sysctlbyname("net.netfil.interface.obyte", &value, &size, &nVal, MemoryLayout<CInt>.size)
        }
    }
    
    
    var list : [String] {
        get {
            var size = -3;
            var usize = 0;
            var ret = sysctlbyname("net.netfil.interface.list", &size, &usize, nil, 0)
            print(ret, size)
            if(ret != 0 || size <= 0) {
                return [String]()
            }
            
            var a = CCArray()
            a.size = Int32(size);
            
            let buffer = UnsafeMutableBufferPointer<UnsafeMutableRawPointer?>(start: UnsafeMutablePointer.allocate(capacity: Int(a.size) * MemoryLayout<UnsafeMutableRawPointer>.size), count: Int(a.size))
            
            for index in 0..<Int(a.size)
            {
                print("\(index)")
                buffer[index] = UnsafeMutableRawPointer.allocate(bytes: Int(MAXCOMLEN), alignedTo: 0)
            }
            a.values = buffer.baseAddress
            
            size = MemoryLayout<CCArray>.size
            
            ret = sysctlbyname("net.netfil.interface.list", &a, &size, nil, 0)
            
            print(ret, a.size)
        
            var retRar = [String]()
            for index in 0..<Int(a.size) {
                let d = a.values[index]!.assumingMemoryBound(to: CChar.self)
                let d2 = UnsafePointer(d)
                let d3 = String(cString: d2)
                retRar.append(d3)
            }
            
            a.deallocate()
            
            return retRar
        }
        set {
            if newValue.count <= 0  {
                return
            }
            
            var a = CCArray()
            a.initialize(from: newValue)
            //var size = 0;
            
            let _ = sysctlbyname("net.netfil.interface.list", nil, nil, &a, MemoryLayout<CCArray>.size)
            
            //a.values.deallocate(capacity: Int(a.size))
            a.deallocate()
            
            
            
            
        }
    }
    
}
