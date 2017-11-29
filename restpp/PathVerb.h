/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/optional.hpp>

namespace librestpp {

	struct PathVerb {
		enum RESTVerb {GET, POST, PUT, PATCH, INVALID};
		PathVerb(const std::string& path, RESTVerb verb) : path(path), verb(verb) {
			wildcard_ = path.find('?') < path.size();
			if (wildcard_) {
				boost::split(parts_, path, boost::is_any_of("/"));
			}
		}

		/**
		 * Note: only for inbound PathVerbs, as wildcards will be wrongly detected
		 * as query parts.
		 */
		PathVerb withoutQuery() const {
			std::vector<std::string> parts;
			boost::split(parts, path, boost::is_any_of("?"));
			return PathVerb(parts[0], verb);
		}

		/**
		 * Note: only for inbound PathVerbs, as wildcards will be wrongly detected
		 * as query parts.
		 */
		std::map<std::string, std::string> getEncodedQueryParts() const {
			std::map<std::string, std::string> result;
			std::vector<std::string> parts;
 			boost::split(parts, path, boost::is_any_of("?"));
			if (parts.size() < 2) {
				return result;
			}
			std::vector<std::string> queries;
			boost::split(queries, parts[1], boost::is_any_of("&"));
			for (auto query : queries) {
				std::vector<std::string> splitQuery;
				boost::split(splitQuery, query, boost::is_any_of("="));
				if (query.size() == 0) {
					continue;
				}
				std::string second;
				if (query.size() > 1) {
					second = splitQuery[1];
				}
				result[splitQuery[0]] = second;
			}
 			return result;
		 }

		bool operator<(const PathVerb& other) const {
			return other.verb == verb ? (path < other.path) : verb < other.verb;
		}

		bool operator==(const PathVerb& other) const {
			return other.verb == verb && path == other.path;
		}

		bool hasWildcard() const {
			return wildcard_;
		}

		boost::optional<std::vector<std::string>> getParameters(const std::string& queryPath) const {
			std::vector<std::string> queryParts;
			boost::split(queryParts, queryPath, boost::is_any_of("/"));
			if (queryParts.size() != parts_.size()) {
				return boost::optional<std::vector<std::string>>();
			}
			std::vector<std::string> results;
			for (size_t i=0; i < parts_.size(); i++) {
				if (parts_[i] == "?") {
					results.push_back(queryParts[i]);
				}
				else if (parts_[i] != queryParts[i]) {
					return boost::optional<std::vector<std::string>>();
				}
			}
			return results;
		}

		std::string path;
		RESTVerb verb;
		private:
			bool wildcard_;
			std::vector<std::string> parts_; // only populated if wildcard_
	};

}
