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

        self.options["assimp/*"].with_x = False
        self.options["assimp/*"].with_3d = False
        self.options["assimp/*"].with_ac = False
        self.options["assimp/*"].with_3ds = False
        self.options["assimp/*"].with_3mf = False
        self.options["assimp/*"].with_amf = False
        self.options["assimp/*"].with_ase = False
        self.options["assimp/*"].with_b3d = False
        self.options["assimp/*"].with_bvh = False
        self.options["assimp/*"].with_cob = False
        self.options["assimp/*"].with_csm = False
        self.options["assimp/*"].with_dxf = False
        self.options["assimp/*"].with_fbx = False
        self.options["assimp/*"].with_hmp = False
        self.options["assimp/*"].with_ifc = False
        self.options["assimp/*"].with_iqm = False
        self.options["assimp/*"].with_irr = False
        self.options["assimp/*"].with_lwo = False
        self.options["assimp/*"].with_lws = False
        self.options["assimp/*"].with_m3d = False
        self.options["assimp/*"].with_md2 = False
        self.options["assimp/*"].with_md3 = False
        self.options["assimp/*"].with_md5 = False
        self.options["assimp/*"].with_mdc = False
        self.options["assimp/*"].with_mdl = False
        self.options["assimp/*"].with_mmd = False
        self.options["assimp/*"].with_ndo = False
        self.options["assimp/*"].with_nff = False
        self.options["assimp/*"].with_obj = True
        self.options["assimp/*"].with_off = False
        self.options["assimp/*"].with_ply = False
        self.options["assimp/*"].with_q3d = False
        self.options["assimp/*"].with_raw = False
        self.options["assimp/*"].with_sib = False
        self.options["assimp/*"].with_smd = False
        self.options["assimp/*"].with_stl = False
        self.options["assimp/*"].with_x3d = False
        self.options["assimp/*"].with_xgl = False
        self.options["assimp/*"].with_gltf = False
        self.options["assimp/*"].with_ms3d = False
        self.options["assimp/*"].with_ogre = False
        self.options["assimp/*"].with_step = False
        self.options["assimp/*"].with_blend = False
        self.options["assimp/*"].with_q3bsp = False
        self.options["assimp/*"].with_assbin = False
        self.options["assimp/*"].with_collada = False
        self.options["assimp/*"].with_irrmesh = False
        self.options["assimp/*"].with_opengex = False
        self.options["assimp/*"].with_terragen = False
        self.options["assimp/*"].with_x_exporter = False
        self.options["assimp/*"].double_precision = False
        self.options["assimp/*"].with_3ds_exporter = False
        self.options["assimp/*"].with_3mf_exporter = False
        self.options["assimp/*"].with_fbx_exporter = False
        self.options["assimp/*"].with_m3d_exporter = False
        self.options["assimp/*"].with_obj_exporter = False
        self.options["assimp/*"].with_ply_exporter = False
        self.options["assimp/*"].with_stl_exporter = False
        self.options["assimp/*"].with_x3d_exporter = False
        self.options["assimp/*"].with_gltf_exporter = False
        self.options["assimp/*"].with_pbrt_exporter = False
        self.options["assimp/*"].with_step_exporter = False
        self.options["assimp/*"].with_assbin_exporter = False
        self.options["assimp/*"].with_assxml_exporter = False
        self.options["assimp/*"].with_assjson_exporter = False
        self.options["assimp/*"].with_collada_exporter = False
        self.options["assimp/*"].with_opengex_exporter = False

    def build_requirements(self):
        self.build_requires("cmake/3.25.0")

    def requirements(self):
        self.requires("assimp/5.4.2")
        self.requires("benchmark/1.9.0")
        self.requires("fmt/11.0.0")
        # self.requires("gperftools/2.16")  # https://github.com/gperftools/gperftools
        self.requires("glfw/3.4")
        self.requires("glm/1.0.1")
        self.requires("gtest/1.15.0")
        self.requires("stb/cci.20240531")
        self.requires("volk/1.3.268.0")

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