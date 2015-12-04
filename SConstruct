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
vars.Add(PackageVariable("openssl", "OpenSSL location", "yes"))
vars.Add("openssl_libnames", "Comma-separated openssl library names to override defaults", None)
vars.Add("openssl_include", "Location of OpenSSL include files (if not under (openssl)/include)", None)
vars.Add("openssl_libdir", "Location of OpenSSL library files (if not under (openssl)/lib)", None)

### presetup

env_ENV = {
    'PATH' : os.environ['PATH'],
    'LD_LIBRARY_PATH' : os.environ.get("LD_LIBRARY_PATH", ""),
    'TERM' : os.environ.get("TERM", ""),
}

if "MSVC_VERSION" in ARGUMENTS :
    env = Environment(ENV = env_ENV, variables = vars, MSVC_VERSION = ARGUMENTS["MSVC_VERSION"], platform = ARGUMENTS.get("PLATFORM", None))
else:
    env = Environment(ENV = env_ENV, variables = vars, platform = ARGUMENTS.get("PLATFORM", None))

if env["PLATFORM"] == "win32" :
    #So we don't need to escalate with UAC
    if "TMP" in os.environ.keys() :
        env['ENV']['TMP'] = os.environ['TMP']

Help(vars.GenerateHelpText(env))

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
        env.Append(CCFLAGS = ["/Zi", "/EHsc"])
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
    env.Append(CXXFLAGS = ["-std=c++11"])

conf_env = env.Clone()
conf = Configure(conf_env)

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

### dependencies

# Boost

boost_conf_env = conf_env.Clone()
boost_flags = {}

if env.get("boost_libdir", None) :
    boost_flags["LIBPATH"] = [Dir(env["boost_libdir"]).abspath]
if env.get("boost_includedir", None) :
    if env["PLATFORM"] == "win32" or env["PLATFORM"] == "hpux" or env["PLATFORM"] == "sunos" :
        boost_flags["CPPPATH"] = [Dir(env["boost_includedir"]).abspath]
    else :
        # Using isystem to avoid getting warnings from a system boost
        # Unfortunately, this also disables dependency tracking
        boost_flags["CPPFLAGS"] = [("-isystem", Dir(env["boost_includedir"]).abspath)]

boost_conf_env.MergeFlags(boost_flags)

conf = Configure(boost_conf_env)

boostLibs = [("system", "system/system_error.hpp"), ("thread", None), ("chrono", None)]
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

# OpenSSL

openssl_env = conf_env.Clone()
use_openssl = bool(env["openssl"])
openssl_prefix = ""
if isinstance(env["openssl"], str) :
    openssl_prefix = env["openssl"]
openssl_flags = {}
if openssl_prefix :
    openssl_include = env.get("openssl_include", None)
    openssl_libdir = env.get("openssl_libdir", None)
    if openssl_include:
        openssl_flags = {"CPPPATH":[openssl_include]}
    else:
        openssl_flags = { "CPPPATH": [os.path.join(openssl_prefix, "include")] }
    if openssl_libdir:
        openssl_flags["LIBPATH"] = [openssl_libdir]
        env["OPENSSL_DIR"] = openssl_prefix
    elif env["PLATFORM"] == "win32" :
        openssl_flags["LIBPATH"] = [os.path.join(openssl_prefix, "lib", "VC")]
        env["OPENSSL_DIR"] = openssl_prefix
    else :
        openssl_flags["LIBPATH"] = [os.path.join(openssl_prefix, "lib")]
    openssl_env.MergeFlags(openssl_flags)

openssl_conf = Configure(openssl_env)
if use_openssl and openssl_conf.CheckCHeader("openssl/ssl.h") :
    found_openssl = 1

openssl_conf.Finish()

if found_openssl:
    boostssl_env = conf_env.Clone()
    boostssl_env.MergeFlags(openssl_flags)
    boostssl_env.MergeFlags(boost_flags)
    boostssl_conf = Configure(boostssl_env)
    if boostssl_conf.CheckCXXHeader("boost/asio/ssl.hpp"):
        env["HAVE_OPENSSL"] = 1
        env["OPENSSL_FLAGS"] = openssl_flags
        openssl_libnames = env.get("openssl_libnames", None)
        if openssl_libnames:
            env["OPENSSL_FLAGS"]["LIBS"] = openssl_libnames.split(',')
        elif env["PLATFORM"] == "win32" :
            env["OPENSSL_FLAGS"]["LIBS"] = ["libeay32MD", "ssleay32MD"]
        else:
            env["OPENSSL_FLAGS"]["LIBS"] = ["ssl", "crypto"]
    else :
        env["OPENSSL_FLAGS"] = {}


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
