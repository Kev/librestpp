# Copyright (c) 2015 Isode Limited.
# All rights reserved.
# See the LICENSE file for more information.

import SCons.SConf
import os


### Parameters

vars = Variables(os.path.join(Dir("#").abspath, "config.py"))
vars.Add('cc', "C compiler")
vars.Add('cxx', "C++ compiler")
vars.Add('ccflags', "Extra C/C++/ObjC compiler flags")
vars.Add('cxxflags', "Extra C++ compiler flags")
vars.Add('link', "Linker")
vars.Add('linkflags', "Extra linker flags")
vars.Add('ar', "Archiver (ar or lib)")
if os.name == "nt":
        vars.Add('mt', "manifest tool")
vars.Add(PathVariable("boost_includedir", "Boost headers location", None, PathVariable.PathAccept))
vars.Add(PathVariable("boost_libdir", "Boost library location", None, PathVariable.PathAccept))
vars.Add(PathVariable("websocketpp_dir", "Websocketpp source location", None, PathVariable.PathAccept))
vars.Add(PathVariable("rapidjson_dir", "RapidJSON source location", None, PathVariable.PathAccept))
vars.Add(BoolVariable("optimize", "Compile with optimizations turned on", "no"))
vars.Add(BoolVariable("debug", "Compile with debug information", "yes"))
vars.Add(BoolVariable("set_iterator_debug_level", "Set _ITERATOR_DEBUG_LEVEL=0", "yes"))

### presetup

env = Environment(variables = vars)
Help(vars.GenerateHelpText(env))
conf_env = env.Clone()
conf = Configure(conf_env)

if "cc" in env :
    env["CC"] = env["cc"]
if "cxx" in env :
    env["CXX"] = env["cxx"]
if "ar" in env :
    env["AR"] = env["ar"]
if "link" in env :
    env["SHLINK"] = env["link"]
    env["LINK"] = env["link"]
for flags_type in ["ccflags", "cxxflags", "linkflags"] :
    if flags_type in env :
        if isinstance(env[flags_type], str) :
            env[flags_type.upper()] = env[flags_type].split(" ")
        else :
            env[flags_type.upper()] = env[flags_type]
if env["optimize"] :
    if env["PLATFORM"] == "win32" :
        env.Append(CCFLAGS = ["/O2"])
    else :
        env.Append(CCFLAGS = ["-O2"])
if env["debug"] :
    if env["PLATFORM"] == "win32" :
        env.Append(CCFLAGS = ["/Zi"])
        env.Append(LINKFLAGS = ["/DEBUG"])
        if GetOption("num_jobs") > 1 :
            env["CCPDBFLAGS"] = '/Fd${TARGET}.pdb'
            env["PDB"] = '${TARGET.base}.pdb'
        if env["set_iterator_debug_level"] :
            env.Append(CPPDEFINES = ["_ITERATOR_DEBUG_LEVEL=0"])
        if env["optimize"] :
            env.Append(LINKFLAGS = ["/OPT:NOREF"])
            env.Append(CCFLAGS = ["/MD"])
        else :
            env.Append(CCFLAGS = ["/MDd"])
    else :
        env.Append(CCFLAGS = ["-g"])
elif env["PLATFORM"] == "win32" :
    env.Append(CCFLAGS = ["/MD"])

if env["PLATFORM"] not in ['win32', 'win64']:
    env.Append(CCFLAGS = ["-std=c++11"])


root = Dir(".").abspath

env.Decider('MD5-timestamp') # Slightly less slow change checking
env.SetOption("max_drift", 1)
env.SetOption("implicit_cache", True)

if not conf.CheckCXX() or not conf.CheckCC() :
    print "Error: You need a working compiler"
    Exit(1)

conf.Finish()

if ARGUMENTS.get("force-configure", 0) :
    SCons.SConf.SetCacheMode("force")

env.AppendUnique(CCFLAGS=['-g'])

### dependencies

# Boost

boost_conf_env = conf_env.Clone()
boost_flags = {}

if env.get("boost_libdir", None) :
    boost_flags["LIBPATH"] = [env["boost_libdir"]]
if env.get("boost_includedir", None) :
    if env["PLATFORM"] == "win32" or env["PLATFORM"] == "hpux" or env["PLATFORM"] == "sunos" :
        boost_flags["CPPPATH"] = [env["boost_includedir"]]
    else :
        # Using isystem to avoid getting warnings from a system boost
        # Unfortunately, this also disables dependency tracking
        boost_flags["CPPFLAGS"] = [("-isystem", env["boost_includedir"])]

boost_conf_env.MergeFlags(boost_flags)

conf = Configure(boost_conf_env)

boostLibs = [("thread", None), ("chrono", None), ("system", "system/system_error.hpp")]
allLibsPresent = True
libNames = []

for (lib, header) in boostLibs :
    if header :
        header = "boost/" + header
    else :
        header = "boost/" + lib + ".hpp"
    if not conf.CheckCXXHeader(header) :
        allLibsPresent = False
        break
    if env["PLATFORM"] != "win32" :
        libName = "boost_" + lib
        if not conf.CheckLib(libName, language='CXX') :
            libName += "-mt"
            if not conf.CheckLib(libName, language='CXX') :
                allLibsPresent = False
                break
        libNames.append(libName)
if allLibsPresent :
    env["BOOST_FLAGS"] = boost_flags
    if env["PLATFORM"] != "win32" :
        env["BOOST_FLAGS"].update({"LIBS": libNames})
else:
    print "Can't find boost"
    Exit(1)

conf.Finish()

# Websocketpp

websocketpp_flags = {}
if env.get("websocketpp_dir", None) :
    websocketpp_flags["CPPPATH"] = [Dir(env['websocketpp_dir']).abspath]
else:
    websocketpp_flags["CPPPATH"] = [Dir("./websocketpp").abspath]

env['WEBSOCKETPP_FLAGS'] = websocketpp_flags

# RapidJSON

rapidjson_flags = {}
if env.get("rapidjson_dir", None) :
    rapidjson_flags["CPPPATH"] = [Dir(env['rapidjson_dir']+'/include').abspath]
else:
    rapidjson_flags["CPPPATH"] = [Dir("./rapidjson/include").abspath]

env['RAPIDJSON_FLAGS'] = rapidjson_flags

### Now run the scripts

Export("env")
Export("conf_env")

# Modules
modules = []
for dir in os.listdir(Dir("#").abspath) :
    if dir in ['rapidjson', 'websocketpp']:
        continue
    full_dir = os.path.join(Dir("#").abspath, dir)
    if not os.path.isdir(full_dir) :
        continue
    sconscript = os.path.join(full_dir, "SConscript")
    if os.path.isfile(sconscript) :
        modules.append(dir)

# Stages
for stage in ["flags", "build"] :
    env["SCONS_STAGE"] = stage
    SConscript(dirs = map(lambda x : root + "/" + x, modules))
