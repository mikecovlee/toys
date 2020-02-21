import network, regex
import codec.base64.standard as base64
import codec.json as json

using network

function http_request(method, host, page, data)
    var sock = new tcp.socket
    runtime.wait_for(10000, []()->sock.connect(tcp.resolve(host, "http")), {})
    @begin
    var head=
        method + " " + page + " HTTP/1.1\r\n" +
        "Host: " + host + "\r\n" +
        "Connection: Close\r\n" +
        "Content-Length: " + data.size + "\r\n" +
        "Accept: */*\r\n" +
        "Pragma: no-cache\r\n" +
        "Cache-Control: no-cache\r\n" +
        "\r\n" + data
    @end
    sock.send(head)
    var response = new string
    try
        loop
            response += sock.receive(32)
        end
    catch e
        sock.close()
        return response
    end
end

function http_response(response)
    var response_header = regex.build("^HTTP/(\\S+) (\\S+) (\\S+)\r\n")
    var header = response_header.match(response)
    #system.out.println("HTTP Version: " + header.str(1))
    #system.out.println("Status Code : " + header.str(2))
    #system.out.println("Description : " + header.str(3))
    var response_data = regex.build("\r\n\r\n")
    var data = response_data.search(response)
    #system.out.println("Content Data:\n" + data.suffix())
    return data.suffix()
end

var source_name = "csman.info"

var platform_name = null
if system.is_platform_windows()
    platform_name = "Win32_MinGW-w64_AMD64"
end
if system.is_platform_linux()
    platform_name = "Linux_GCC_AMD64"
end
if system.is_platform_darwin()
    platform_name = "macOS_clang_AMD64"
end

var CSMAN = json.to_var(json.from_string(http_response(http_request("GET", source_name, "/csman.json", ""))))
system.out.println("CSMAN Version: " + CSMAN.Version)

function query_platform(name)
    foreach it in CSMAN.Platform
        if it == name
            return true
        end
    end
    return false
end

if !query_platform(platform_name)
    throw runtime.exception("Platform " + platform_name + " do not support!")
end

var url_parser = regex.build("^http://(\\S+)/(\\S+)$")
var result = url_parser.match(CSMAN.BaseUrl)
source_name = result.str(1)
var base_url = "/" + result.str(2)

var Record = json.to_var(json.from_string(http_response(http_request("GET", source_name, base_url + platform_name + ".json", ""))))

if query_platform("Generic")
    var Generic = json.to_var(json.from_string(http_response(http_request("GET", source_name, base_url + "Generic.json", ""))))
    foreach it in Generic
        if !Record.exist(it.first)
            Record.insert(it.first, new hash_map)
        end
        var map = Record[it.first]
        foreach rec in it.second
            if map.exist(rec.first)
                throw runtime.exception("Package collision.")
            end
            map.insert(rec.first, rec.second)
        end
        swap(map, Record[it.first])
    end
end

foreach it in Record
    system.out.println(it.first + ":")
    foreach rec in it.second
        system.out.print("\t" + rec.first)
        system.out.println(":\t" + rec.second.Description)
    end
end