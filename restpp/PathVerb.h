/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#pragma once

namespace librestpp {

	struct PathVerb {
		enum RESTVerb {GET, POST, INVALID};
		PathVerb(const std::string& path, RESTVerb verb) : path(path), verb(verb) {}
		bool operator<(const PathVerb& other) const
		{
			return other.verb == verb ? (path < other.path) : verb < other.verb;
		}
		bool operator==(const PathVerb& other) const
		{
			return other.verb == verb && path == other.path;
		}

		std::string path;
		RESTVerb verb;
	};

}
