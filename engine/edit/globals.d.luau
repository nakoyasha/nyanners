--#METADATA#{"CREATABLE_INSTANCES": ["ImageLabel", "TextLabel", "Script", "Button", "Sound"]}

--# selene: allow(allow_error)
declare engine: {
    version: string,
    quit : () -> (),
    connectToUpdate: (any, callback : () -> ()) -> (),
}



declare immediate: {
    -- Starts a new ImGui window.
    startImmediate: (windowName : string, flags : number?) -> (),

    -- Displays a textlabel on the window.
    text: (text : string) -> (),

    -- Returns the user-provided string, and a boolean indicating whether it was changed.
    inputText: (text : string) -> (string, boolean),

    -- Displays a button, obviously.
    button: (text : string) -> boolean,

    -- ImGui::Combo
    combo: (label : string, items : {string}) -> (number, boolean),

    getWindowSize: () -> vector,
    setWindowSize: (vector) -> (),

    getWindowPosition: () -> vector,
    setWindowPosition: (vector) -> (),

    -- Ends the current ImGui draw.
    endImmediate: () -> (),

    flags: {
        None: number,
        NoTitleBar: number,
        NoResize: number,
        NoMove: number,
        NoScrollbar: number,
        NoScrollWithMouse: number,
        NoCollapse: number,
        AlwaysAutoResize: number,
        NoBackground: number,
        NoSavedSettings: number,
        NoMouseInputs: number,
        MenuBar: number,
        HorizontalScrollbar: number,
        NoFocusOnAppearing: number,
        NoBringToFrontOnFocus: number,
        AlwaysVerticalScrollbar: number,
        AlwaysHorizontalScrollbar: number,
        NoNavInputs: number,
        NoNavFocus: number,
        UnsavedDocument: number,
    },
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

declare class FileService extends Instance
    function fileExists(self, path : string) : boolean
    function readFile(self, path : string) : string
    function writeFile(self, path : string, content : string) : string
end

export type Signal<T... = ...any> = {
    Connect: (Signal<T...>, callback: (T...) -> ()) -> number,
}

export type Service = HttpService | FileService
declare class DataModel extends Instance
    Tick : Signal<number>
    OnDraw : Signal<number>
    function getService(self, service: "HttpService" | "Workspace" | "ScriptService" | "FileService") : Service
    function setFPS(self, fps : number) : nil
    function GetFPS(self) : number
    function SetWindowTitle(self, title : string) : nil
    function SetWindowSize(self, size : vector) : nil
    function SetWindowIcon(self, image : string) : nil
    function SetWindowFlags(self, flags : number) : nil
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