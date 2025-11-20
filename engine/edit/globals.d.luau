--# selene: allow(allow_error)
declare engine: {
    version: string,
    quit : () -> (),
    connectToUpdate: (any, callback : () -> ()) -> (),
}

declare engine_DispatchNative: (message : string) -> ()
declare engine_DrawText: (message : string) -> ()
declare engine_panic: (message : string) -> ()