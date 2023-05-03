# 用于编译特定的三方库模块，$1为需要编译的三方库模块的文件夹与根目录的相对路径
import os
import sys
import subprocess
import abc

# 编译库的抽象基类建造者
class AbstractBuilder(abc.ABC):
    # 配置前处理
    @abc.abstractmethod
    def preCommand(self) -> None:
        pass

    # 配置构建
    @abc.abstractmethod
    def configurateBuild(self) -> None:
        pass

    # 进行构建
    @abc.abstractmethod
    def build(self) -> None:
        pass

    # 构建后处理
    @abc.abstractmethod
    def postCommand(self) -> None:
        pass


# 建造者主控，用于进行建造
class BuildDirector:
    # 选择建造模式
    def __init__(self, build_mode: AbstractBuilder ) -> None:
        self.__current_build_mode__ = build_mode

    # 进行相应构造
    def runBuild(self) -> None:
        self.__current_build_mode__.preCommand()
        self.__current_build_mode__.configurateBuild()
        self.__current_build_mode__.build()
        self.__current_build_mode__.postCommand()
        self.outputInfo()
    
    # 输出编译信息
    def outputInfo(self):
        print(f"third_party_path: {self.__current_build_mode__.third_party_path_}")
        print(f"build_path: {self.__current_build_mode__.build_path_}")
        print(f"install_path: {self.__current_build_mode__.install_path_}")
        print(f"system_name: {self.__current_build_mode__.system_name_}")
        print(f"cxx_compiler_id: {self.__current_build_mode__.cxx_compiler_id_}")
        print(f"build_tool: {self.__current_build_mode__.build_tool_}")
        print(f"build_type: {self.__current_build_mode__.build_type_}")
        print(f"cmake_args: {self.__current_build_mode__.cmake_args_}")
        print(f"make_args: {self.__current_build_mode__.make_args_}")


# 基础的编译库建造者，为抽象基类，也是一个基础的模板方法
class BaseBuilder(AbstractBuilder):
    # 读取参数进行路径等数据的初始化
    def __init__(self, target_path : str, system_name: str, cxx_compiler_id: str, build_tool : str, build_type : str) -> None:
        self.original_target_name_ = target_path
        target_path = os.path.abspath(target_path)
        self.third_party_path_ = os.path.join(target_path, "..")
        self.build_path_ = os.path.join(target_path, "build")
        self.install_path_ = os.path.normpath(f'{self.build_path_}/install')
        self.system_name_ = system_name
        self.cxx_compiler_id_ = cxx_compiler_id
        self.build_tool_ = build_tool
        self.build_type_ = build_type
        self.cmd_concat_flag_ = '&' if system_name.find("Windows") != -1 else ';'
        if self.system_name_.find("Windows") != -1:
            # 微软c++工具集的各种环境设置，包括msbuild，nmake等构建工具
            self.msbuild_setenv_path_ = os.path.normpath(f"{os.getenv('VS2019INSTALLDIR')}/VC/Auxiliary/Build/vcvarsamd64_x86.bat")
            # intel编译器各种环境设置
            self.icl_setenv_path_ = os.path.normpath(f"{os.getenv('ONEAPI_ROOT')}/setvars.bat")
        print(f"third_party_path: {self.third_party_path_}")
        print(f"build_path: {self.build_path_}")
        print(f"install_path: {self.install_path_}")
        print(f"system_name: {self.system_name_}")
        print(f"cxx_compiler_id: {self.cxx_compiler_id_}")
        print(f"build_tool: {self.build_tool_}")
        print(f"build_type: {self.build_type_}")

    def preCommand(self) -> None:
        pass

    # 配置构建中需要先设置的环境
    @abc.abstractmethod
    def setEnvBeforeConfig(self):
        pass

    # 配置构建中的特殊步骤
    @abc.abstractmethod
    def configurateOthers(self) -> None:
        pass

    def configurateBuild(self) -> None:
        # 创建并切换到构建路径
        os.makedirs(self.build_path_, exist_ok=True)
        os.chdir(self.build_path_)
        # cmake配置选项
        self.cmake_args_ = ["cmake", "..", f"-DCMAKE_BUILD_TYPE={self.build_type_}", f"-DCMAKE_CONFIGURATION_TYPES={self.build_type_}"]
        # 其他配置
        self.setEnvBeforeConfig()
        self.configurateOthers()
        print(f"cmake_args: {self.cmake_args_}")
        # 生成构建文件
        ret = subprocess.run(self.cmake_args_, shell=True)
        if ret.returncode != 0:
            exit(1)

    # 构建中需要先设置的环境
    @abc.abstractmethod
    def setEnvBeforeBuild(self):
        pass

    # 构建中的特殊步骤
    @abc.abstractmethod
    def buildOthers(self) -> None:
        pass

    def build(self) -> None:
        # 创建并切换到构建路径
        os.makedirs(self.build_path_, exist_ok=True)
        os.chdir(self.build_path_)
        # 默认构建选项，直接调用构建配置文件命令中默认或指定的构建工具进行构建
        self.make_args_ = ["cmake", "--build", ".", "-j", "--config", self.build_type_]
        # 其他构建
        self.setEnvBeforeBuild()
        self.buildOthers()
        print(f"make_args: {self.make_args_}")
        # 进行构建
        subprocess.run(self.make_args_, shell=True)

    def postCommand(self) -> None:
        pass


# 通用的编译库建造者，里面有一些特殊的静态操作。
# 无特殊步骤的库都可以用该建造者进行构建。
class CommonBuilder(BaseBuilder):
    @staticmethod
    def installPerlOnWindows(perl_msi_path: str, perl_msi_install_path: str) -> None:
        # 静默安装perl程序，用于读取openssl的配置以进行构建
        if not os.path.exists(perl_msi_install_path):
            prerun_args = ["msiexec", "/i", perl_msi_path, f"INSTALLDIR={perl_msi_install_path}", "/qb!", "/l*v", "strawberry-perl_install.log"]
            print(f"prerun_args: {prerun_args}")
            ret = subprocess.run(prerun_args, shell=True)
            if ret.returncode != 0:
                exit(1)
        else:
            print(f"perl has been installed in {perl_msi_install_path}")
    
    @staticmethod
    def useIntelCompiler(cmake_args: list, cmd_concat_flag: str, icl_setenv_path: str) -> None:
        # 需要用到intel编译器各种环境设置
        # 各种环境设置添加到cmake配置选项
        cmake_args.insert(0, cmd_concat_flag)
        cmake_args.insert(0, icl_setenv_path)
        cmake_args += ["-T", "Intel C++ Compiler 2022", '-DCMAKE_C_COMPILER="icl"', '-DCMAKE_CXX_COMPILER="icl"']

    def setEnvBeforeConfig(self) -> None:
        pass
    
    def setEnvBeforeBuild(self) -> None:
        pass
    
    def configurateOthers(self) -> None:
        pass

    def buildOthers(self) -> None:
        pass


# 用intel编译器时的编译建造者
# 用intel编译器进行库的编译
class IntelCompilerBuilder(CommonBuilder):
    def setEnvBeforeConfig(self) -> None:
        if self.system_name_.find("Windows") != -1 and self.cxx_compiler_id_.find("intel") != -1:
            # 需要用到intel编译器各种环境设置
            self.useIntelCompiler(self.cmake_args_, self.cmd_concat_flag_, self.icl_setenv_path_)
    
    def setEnvBeforeBuild(self) -> None:
        if self.system_name_.find("Windows") != -1 and self.cxx_compiler_id_.find("intel") != -1:
            # 各种环境设置添加到构建选项
            self.make_args_.insert(0, self.cmd_concat_flag_)
            self.make_args_.insert(0, self.icl_setenv_path_)

# googletest库的建造者
class GoogletestBuilder(IntelCompilerBuilder):
    def setEnvBeforeConfig(self) -> None:
        if self.system_name_.find("Windows") != -1 and self.cxx_compiler_id_.find("intel") != -1:
            # 需要用到intel编译器各种环境设置
            self.useIntelCompiler(self.cmake_args_, self.cmd_concat_flag_, self.icl_setenv_path_)
        # 强制让gtest链接动态运行时库（shared (DLL) run-time lib）而不管gtest编译生成的是staitc还是shared库
        # 用于解决用msvc编译生成的gtest staitc库时出现的MD/MDd不匹配的链接错误
        # 此时在项目的MD/MDd和MT/MTd参数下都能使用生成的gtest的任何static/shared库
        self.cmake_args_.append("-Dgtest_force_shared_crt=1")

# boost库的建造者
class BoostBuilder(CommonBuilder):
    # boost的构建配置
    def configurateOthers(self) -> None:
        self.cmake_args_ = ["cd", os.path.normpath(f"{self.build_path_}/.."), self.cmd_concat_flag_]
        if self.system_name_.find("Windows") != -1:
            self.cmake_args_.append(os.path.normpath(".\\bootstrap.bat"))
        elif self.system_name_.find("Linux") != -1:
            self.cmake_args_.append(os.path.normpath("./bootstrap.sh"))

    # boost的构建
    def buildOthers(self) -> None:
        self.make_args_ = ["cd", os.path.normpath(f"{self.build_path_}/..")]
        compile_args = [self.cmd_concat_flag_, os.path.normpath("./b2"), f"--prefix={self.build_path_}", f"--stagedir={self.build_path_}", "--without-python", "--without-graph_parallel", "--without-mpi", f"variant={self.build_type_.lower()}", "address-model=64", "architecture=x86", "threading=multi"]
        static_compile_args = ["link=static", "runtime-link=shared"]
        shared_compile_args = ["link=shared", "runtime-link=shared"]
        # 用msvc（因为用到boost的其他三方库的编译只能识别到msvc版本）编译boost静态库
        self.make_args_ += compile_args + static_compile_args + ["install"]
        # windows上要额外用intel编译器编译
        if self.system_name_.find("Windows") != -1:
            # 需要用到intel编译器各种环境设置
            self.make_args_.insert(0, self.cmd_concat_flag_)
            self.make_args_.insert(0, self.icl_setenv_path_)
            compile_args.append("toolset=intel-2021.1-vc14.1")
        # 用intel编译boost动态库
        self.make_args_ += compile_args + shared_compile_args + ["stage"]


# hdf5/hdf5_rev库的建造者
class HDF5Builder(IntelCompilerBuilder):
    # Windows上需要先安装perl
    def preCommand(self) -> None:
        if self.system_name_.find("Windows") != -1:
            self.installPerlOnWindows(os.path.normpath(f"{self.build_path_}/../strawberry-perl-5.32.1.1-64bit.msi"), os.path.normpath(f"{os.getenv('LOCALAPPDATA')}/Programs/strawberry-perl"))

    # 只用到intel的编译环境，因为需要用到intel fortran编译器
    def setEnvBeforeConfig(self) -> None:
        if self.system_name_.find("Windows") != -1 and self.cxx_compiler_id_.find("intel") != -1:
            # 需要用到intel fortran编译器的各种环境设置
            # 各种环境设置添加到cmake配置选项
            self.cmake_args_.insert(0, self.cmd_concat_flag_)
            self.cmake_args_.insert(0, self.icl_setenv_path_)

    # hdf5/hdf5_rev的构建配置
    def configurateOthers(self) -> None:
        # 配置同时编译静态和动态库
        # 指定v110版本的api接口
        self.cmake_args_ += ["-C", os.path.normpath("../config/cmake/cacheinit.cmake"), "-DBUILD_SHARED_LIBS:BOOL=ON", "-DBUILD_TESTING:BOOL=ON", "-DHDF5_BUILD_TOOLS:BOOL=ON", "-DHDF5_ENABLE_SZIP_SUPPORT:BOOL=OFF", "-DHDF5_ENABLE_Z_LIB_SUPPORT:BOOL=OFF", "-DDEFAULT_API_VERSION=v110"]
    
    # hdf5/hdf5_rev的构建
    def buildOthers(self) -> None:
        self.make_args_ += [self.cmd_concat_flag_, "cmake", "--install", self.build_path_, "--config", self.build_type_, "--prefix", self.install_path_]


# breakpad库的建造者
class BreakpadBuilder(CommonBuilder): 
    # breakpad不需要构建，cmake中已调用子进程进行构建
    def buildOthers(self) -> None:
        self.make_args_ = []


# openssl库的建造者
class OpensslBuilder(CommonBuilder):
    # Windows上需要先安装perl
    def preCommand(self) -> None:
        if self.system_name_.find("Windows") != -1:
            self.installPerlOnWindows(os.path.normpath(f"{self.build_path_}/../strawberry-perl-5.32.1.1-64bit.msi"), os.path.normpath(f"{os.getenv('LOCALAPPDATA')}/Programs/strawberry-perl"))

    # openssl的构建配置
    def configurateOthers(self) -> None:
        # 读取openssl的配置以进行构建
        self.cmake_args_ = ["perl", os.path.normpath(f"../Configure"), "VC-WIN32", "no-asm", "no-shared", f"--prefix={self.build_path_}"]
    
    # openssl的构建
    def buildOthers(self) -> None:
        self.make_args_ = [self.msbuild_setenv_path_, self.cmd_concat_flag_, 'nmake', self.cmd_concat_flag_, 'nmake', 'install']


# libevent库的建造者
class LibeventBuilder(IntelCompilerBuilder):
    # libevent的构建配置
    def configurateOthers(self) -> None:
        # 读取openssl的配置以进行构建
        self.cmake_args_.append(f"-DOPENSSL_ROOT_DIR={os.path.normpath(self.third_party_path_+'/openssl/build')}")


# thrift库的建造者
class ThriftBuilder(CommonBuilder):
    # 库中已存在build文件夹，因此改为build_
    def __init__(self, target_path : str, system_name: str, cxx_compiler_id: str, build_tool : str, build_type : str) -> None:
        super().__init__(target_path, system_name, cxx_compiler_id, build_tool, build_type)
        target_path = os.path.abspath(target_path)
        self.build_path_ = os.path.join(target_path, "build_")
        self.install_path_ = os.path.normpath(f'{self.build_path_}/install')
        print(f"new build_path: {self.build_path_}")
        print(f"new install_path: {self.install_path_}")

    # thrift的构建配置
    def configurateOthers(self) -> None:
        if self.system_name_.find("Windows") != -1:
            os.environ['Path'] = os.getenv('Path') + os.pathsep + os.path.normpath(self.third_party_path_+"/win_flex_bison")
        self.cmake_args_ += [f"-DBOOST_ROOT={os.path.normpath(self.third_party_path_+'/boost/build')}", f"-DBOOST_LIBRARYDIR={os.path.normpath(self.third_party_path_+'/boost/build/lib')}", f"-DOPENSSL_ROOT_DIR={os.path.normpath(self.third_party_path_+'/openssl')}", f"-DLIBEVENT_ROOT={os.path.normpath(self.third_party_path_+'/libevent')}"]


# 三方库编译类
class ModuleCompilation:
    # 建造者列表
    __builders__ = [CommonBuilder, IntelCompilerBuilder, GoogletestBuilder, BoostBuilder, HDF5Builder, BreakpadBuilder, OpensslBuilder, LibeventBuilder, ThriftBuilder]
    def __init__(self, target_path : str, system_name: str, cxx_compiler_id: str, build_tool : str, build_type : str):
        has_found = False
        # 有专有建造者的情况下用专有的
        for builder in ModuleCompilation.__builders__:
            builder_name = builder.__name__.replace("Builder", "").lower()
            if target_path.find(builder_name) != -1:
                print(builder_name)
                has_found = True
                ModuleCompilation.executeBuild(target_path, system_name, cxx_compiler_id, build_tool, build_type, builder)
                break
        # 没有专有建造者的情况下调用intel编译器的建造者
        if not has_found:
            self.executeBuild(target_path, system_name, cxx_compiler_id, build_tool, build_type, IntelCompilerBuilder)
    
    @staticmethod
    def executeBuild(target_path : str, system_name: str, cxx_compiler_id: str, build_tool : str, build_type : str, builder: AbstractBuilder):
        builder_obj = builder(target_path, system_name, cxx_compiler_id, build_tool, build_type)
        executive = BuildDirector(builder_obj)
        executive.runBuild()
        print(f'{builder.__name__} has been called and executed')


# 获取命令行参数
if __name__ == '__main__':
    if len(sys.argv) < 6 or not os.path.exists(sys.argv[1]):
        print("invalid input!")
        exit(1)
    else:
        ModuleCompilation(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5])


# a = AbstractBuilder("D:\Documents\code\pera_core", "Linux", "intel", "make", "Debug")
# a = BaseBuilder("D:\Documents\code\pera_core", "Linux", "intel", "make", "Debug")
