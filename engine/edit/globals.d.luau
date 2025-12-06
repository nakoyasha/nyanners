--#METADATA#{"CREATABLE_INSTANCES": ["ImageLabel", "TextLabel", "Script", "Button", "Sound"]}

--# selene: allow(allow_error)
declare engine: {
    version: string,
    quit : () -> (),
    connectToUpdate: (any, callback : () -> ()) -> (),
}

declare class Instance
    Name : string
    Parent : Instance?
    ClassName : string
    function IsA(self, className : string) : boolean
    function Destroy(self) : nil
end

declare class UIDrawable extends Instance
    Position : any
    Size : any
end

declare class ImageLabel extends UIDrawable
    Texture : string
end

declare class TextLabel extends UIDrawable
    Text : string
end

declare class Sound extends Instance
    Looped : boolean
    IsPlaying : boolean
    Sound : string
    function Play (self) : nil
    function Stop (self) : nil
    function Pause (self) : nil
    function Resume (self) : nil
end

declare class HttpService extends Instance
    function request(self, url: string, path : string) : string
    function JSONDecode(self, json : string) : {[string]: number | boolean | string | {any}}
end

export type Signal<T... = ...any> = {
    Connect: (Signal<T...>, callback: (T...) -> ()) -> number,
}

export type Service = HttpService
declare class DataModel extends Instance
    Tick : Signal<number>
    function getService(self, service: "HttpService") : Service
    function setFPS(self, fps : number) : nil
    function SetWindowTitle(self, title : string) : nil
end

declare class Script extends Instance
    Source : string
end


declare game: DataModel
declare script : Script
declare Instance : {
    new: ((className: string) -> Instance),
}

declare engine_DispatchNative: (message : string) -> ()
declare engine_DrawText: (message : string) -> ()
declare engine_panic: (message : string) -> ()