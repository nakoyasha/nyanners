# Nyanners

very basic c++ game engine that i'll probably never finish. loosely based on how roblox is built.

currently supports windows* and linux, macos will NEVER be supported (i don't like apple)

*windows compilation is complicated due to use of raylib and httplib (httplib imports from winsock, which conflicts with raylib)
i have not tried yet but setting NO_HTTP_SERVICE should get something building.