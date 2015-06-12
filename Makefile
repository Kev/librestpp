# Copyright (c) 2015 Isode Limited.
# All rights reserved.
# See the LICENSE file for more information.

BOOST = /usr/local/brew/Cellar/boost/1.57.0
RAPIDJSON = rapidjson

all:
	scons --implicit-cache

.PHONY: systemchanged
systemchanged:
	scons --implicit-deps-changed

.PHONY: clean
clean:
	scons -c


websocketpp/.git:
	git submodule init && git submodule update
