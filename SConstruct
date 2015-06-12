# Copyright (c) 2015 Isode Limited.
# All rights reserved.
# See the LICENSE file for more information.

import SCons.SConf
import os

### presetup

env = Environment()
conf_env = env.Clone()
conf = Configure(conf_env)

root = Dir(".").abspath

Decider('MD5-timestamp') # Slightly less slow change checking

if not conf.CheckCXX() or not conf.CheckCC() :
    print "Error: You need a working compiler"
    Exit(1)

if ARGUMENTS.get("force-configure", 0) :
    SCons.SConf.SetCacheMode("force")

env.AppendUnique(CCFLAGS=['-g'])

### dependencies

# Boost

boost_conf_env = conf_env.Clone()
boost_flags = {}

boost_dir = '/usr/local/brew/Cellar/boost/1.57.0/'
env['boost_libdir'] = boost_dir + 'lib'
env['boost_includedir'] = boost_dir + 'include'

if env.get("boost_libdir", None) :
    boost_flags["LIBPATH"] = [env["boost_libdir"]]
if env.get("boost_includedir", None) :
    if env["PLATFORM"] == "win32" or env["PLATFORM"] == "hpux" or env["PLATFORM"] == "sunos" :
        boost_flags["CPPPATH"] = [env["boost_includedir"]]
    else :
        # Using isystem to avoid getting warnings from a system boost
        # Unfortunately, this also disables dependency tracking
        boost_flags["CPPFLAGS"] = [("-isystem", env["boost_includedir"])]


boostLibs = [("thread-mt", None), ("chrono", None), ("system", "system/system_error.hpp")]
allLibsPresent = True
libNames = []

for (lib, header) in boostLibs :
    if header :
        header = "boost/" + header
    else :
        header = "boost/" + lib + ".hpp"
    libName = "boost_" + lib
    libNames.append(libName)
if allLibsPresent :
    env["BOOST_FLAGS"] = boost_flags
    if env["PLATFORM"] != "win32" :
        env["BOOST_FLAGS"].update({"LIBS": libNames})
else:
    print "Can't find boost"
    Exit(1)

# Websocketpp

websocketpp_flags = {}
websocketpp_flags["CPPPATH"] = [Dir("./websocketpp").abspath]

env['WEBSOCKETPP_FLAGS'] = websocketpp_flags

# RapidJSON

rapidjson_flags = {}
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

# Flags
for stage in ["flags", "build"] :
    env["SCONS_STAGE"] = stage
    SConscript(dirs = map(lambda x : root + "/" + x, modules))
