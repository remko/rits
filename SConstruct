import os, platform

# Config
vars = Variables("config.py")
vars.Add(BoolVariable("optimize", "Compile with optimizations turned on", "no"))
vars.Add(BoolVariable("debug", "Compile with debug information", "yes"))
vars.Add(BoolVariable("check", "Run unit tests", "no"))

env = Environment(ENV = {'PATH': os.environ['PATH']}, variables = vars)
Help(vars.GenerateHelpText(env))

# Compiler
env["CXXFLAGS"] = ["-std=c++11"]
if env["debug"] :
	env.Append(CCFLAGS = ["-g"])
if env["optimize"] :
	env.Append(CCFLAGS = ["-O2"])
if env["PLATFORM"] == "posix" :
	env.Append(LIBS = ["pthread"])
	env.Append(CXXFLAGS = ["-Wextra", "-Wall"])
elif env["PLATFORM"] == "darwin" :
	env["CC"] = "clang"
	env["CXX"] = "clang++"
	env["LINK"] = "clang++"
	if platform.machine() == "x86_64" :
		env["CCFLAGS"] = ["-arch", "x86_64"]
		env.Append(LINKFLAGS = ["-arch", "x86_64"])
	env.Append(CXXFLAGS = ["-Wall", "-Wextra", "-Wno-unused-parameter"])


# LibXML2
env.ParseConfig('xml2-config --cflags --libs')

# Boost
env["BOOST_DIR"] = "/usr/local/homebrew/opt/boost"
env.Append(CPPDEFINES = ["BOOST_LOG_DYN_LINK", "BOOST_LOG_NO_THREADS"])
env.Append(CPPPATH = ["${BOOST_DIR}/include"])
env.Append(LIBPATH = ["${BOOST_DIR}/lib"])
env.Append(LIBS = ["boost_filesystem", "boost_system", "boost_log", "pthread"])

# TagLib
env.ParseConfig('taglib-config --cflags --libs')

lib = env.StaticLibrary("src/rits", [
	"src/parser.cpp",
	"src/tagger.cpp",
	"src/encoder.cpp",
	"src/uridecode.cpp",
	"src/Library.cpp",
	"src/Song.cpp"
])

env.Program("bin/itunes-shrink", [ "src/itunes-shrink.cpp", lib ])
env.Program("bin/itunes-foster", [ "src/itunes-foster.cpp", lib ])
