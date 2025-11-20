--# selene: allow(allow_error)
declare engine: {
    version: string,
    quit : () -> (),
    connectToUpdate: (any, callback : () -> ()) -> (),
}

type Instance =  {
    Name : string,
    Parent : Instance,
    ClassName : string,
    IsA : (Instance, className : string) -> boolean,
    Destroy: (Instance) -> (),
}

type HttpService = Instance & {
    request : (HttpService, url : string, path : string) -> string,
    JSONDecode : (HttpService, json : string) -> {[string]: number | boolean | string | {any}}
}

type Service = HttpService
type DataModel = Instance & {
    getService: (DataModel, service : "HttpService") -> Service,
    setFPS: (DataModel, fps : number) -> (),
}
type Script = Instance & {
    Source : string,
}

declare game: DataModel
declare script : Script

declare engine_DispatchNative: (message : string) -> ()
declare engine_DrawText: (message : string) -> ()
declare engine_panic: (message : string) -> ()