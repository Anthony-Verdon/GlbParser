set_languages("cxx17")

set_toolchains("clang")
set_warnings("allextra", "error")
set_optimize("fastest")
set_symbols("debug")

namespace("Matrix", function ()
    includes("submodules/Matrix")
end)

namespace("Json", function ()
    includes("submodules/Json")
end)

target("GlbParser")
    set_targetdir("./")
    set_kind("static")
    add_files("srcs/**.cpp")
    add_deps("Json::Json")
    add_deps("Matrix::Matrix")
    add_includedirs("srcs", {public = true})