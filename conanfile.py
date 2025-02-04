from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.build import cross_building


class XarEngine(ConanFile):
    name = "Xar Engine"
    version = 0.0
    author = "michal.wendel@gmail.com"

    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    options = {
        "shared": [True, False],
    }
    default_options = {
        "shared": False,
    }

    def configure(self):
        if cross_building(self):
            self.output.warn("Cross-building detected")

    def build_requirements(self):
        self.build_requires("cmake/3.25.0")

    def requirements(self):
        self.requires("benchmark/1.9.0")
        self.requires("fmt/11.0.0")
        # self.requires("gperftools/2.16")  # https://github.com/gperftools/gperftools
        self.requires("glfw/3.4")
        self.requires("gtest/1.15.0")

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package_info(self):
        self.cpp_info.libs = ["xar_engine"]

    def package(self):
        cmake = CMake(self)
        cmake.install()