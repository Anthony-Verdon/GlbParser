set_targetdir("./")
set_languages("cxx17")

set_toolchains("clang")
set_warnings("allextra", "error")
set_optimize("fastest")
set_symbols("debug")

namespace("Toolbox")
includes("submodules/Toolbox")
namespace_end()

namespace("Matrix")
includes("submodules/Matrix")
namespace_end()

namespace("Json")
includes("submodules/Json")
namespace_end()

target("GlbParser")
    set_kind("static")
    add_files("srcs/**.cpp")
    add_deps("Toolbox::Toolbox")
    add_deps("Json::Json")
    add_deps("Matrix::Matrix")
    add_includedirs("srcs", {public = true})