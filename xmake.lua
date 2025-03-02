set_targetdir("./")
set_languages("cxx17")

set_toolchains("clang")
add_ldflags("-stdlib=libc++")
add_cxxflags("-std=c++17")
add_cxxflags("-Wall")
add_cxxflags("-Werror")
add_cxxflags("-Wextra")
add_cxxflags("-O3")

add_requires("nlohmann_json")

target("exe")
    set_kind("binary")
    add_files("srcs/**.cpp")
    add_includedirs("srcs")
    add_packages("nlohmann_json")