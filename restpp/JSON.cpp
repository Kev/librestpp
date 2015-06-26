/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#include "JSON.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <boost/make_shared.hpp>

namespace librestpp {

JSONValue::~JSONValue() {

}

JSONInt::JSONInt(int value) : value_(value) {

}

int JSONInt::getValue() {
	return value_;
}

JSONValue::ref JSONInt::set(int value) {
	value_ = value;
	return shared_from_this();
}

JSONString::JSONString(const std::string& value) : value_(value) {

}


JSONValue::ref JSONString::set(const std::string& value) {
	value_ = value;
	return shared_from_this();
}

std::string JSONString::getValue() {
	return value_;
}

JSONBool::JSONBool(bool value) : value_(value) {

}


JSONValue::ref JSONBool::set(bool value) {
	value_ = value;
	return shared_from_this();
}

bool JSONBool::getValue() {
	return value_;
}

JSONArray::JSONArray() {

}

JSONValue::ref JSONArray::append(JSONValue::ref value) {
	values_.push_back(value);
	return shared_from_this();
}

std::vector<JSONValue::ref> JSONArray::getValues() {
	return values_;
}

JSONObject::JSONObject() {

}

JSONValue::ref JSONObject::set(const std::string& key, JSONValue::ref value) {
	values_[key] = value;
	return shared_from_this();
}

std::map<std::string, JSONValue::ref> JSONObject::getValues() {
	return values_;
}

void jsonValueToRapidJSON(JSONValue* value, rapidjson::Value& rapidValue, rapidjson::Document& document) {
	JSONInt* intValue = dynamic_cast<JSONInt*>(value);
	if (intValue) {
		rapidValue.SetInt(intValue->getValue());
		return;
	}
	JSONString* stringValue = dynamic_cast<JSONString*>(value);
	if (stringValue) {
		rapidValue.SetString(stringValue->getValue().c_str(), stringValue->getValue().size(), document.GetAllocator());
		return;
	}
	JSONBool* boolValue = dynamic_cast<JSONBool*>(value);
	if (boolValue) {
		rapidValue.SetBool(boolValue->getValue());
		return;
	}
	JSONArray* arrayValue = dynamic_cast<JSONArray*>(value);
	if (arrayValue) {
		rapidValue.SetArray();
		std::vector<JSONValue::ref> values = arrayValue->getValues();
		for (size_t i = 0; i < values.size(); i++) {
			rapidjson::Value obj;
			jsonValueToRapidJSON(values[i].get(), obj, document);
			rapidValue.PushBack(obj, document.GetAllocator());
		}
		return;
	}
	JSONObject* objectValue = dynamic_cast<JSONObject*>(value);
	if (objectValue) {
		rapidValue.SetObject();
		typedef std::map<std::string, JSONValue::ref> ValuesMap;
		ValuesMap values = objectValue->getValues();
		for (ValuesMap::iterator it = values.begin(); it != values.end(); it++) {
			rapidjson::Value obj;
			jsonValueToRapidJSON(it->second.get(), obj, document);
			rapidjson::Value key;
			key.SetString(it->first.c_str(), it->first.size(), document.GetAllocator());
			rapidValue.AddMember(key, obj, document.GetAllocator());
		}
		return;
	}
	assert(false);
}

std::string JSONObject::serialize() {
	rapidjson::Document d;
	jsonValueToRapidJSON(this, d, d);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	return buffer.GetString();
}

boost::shared_ptr<JSONValue> convertRapidJSON(const rapidjson::Value* json, size_t depth) {
	if (depth > 20) {
		/* stack overflow attack*/
		return boost::shared_ptr<JSONValue>();
	}
	if (json->IsBool()) {
		return boost::make_shared<JSONBool>(json->GetBool());
	}
	if (json->IsInt()) {
		return boost::make_shared<JSONInt>(json->GetInt());
	}
	if (json->IsString()) {
		return boost::make_shared<JSONString>(json->GetString());
	}
	if (json->IsArray()) {
		boost::shared_ptr<JSONArray> array = boost::make_shared<JSONArray>();
		for (size_t i = 0; i < json->Size(); i++) {
			array->append(convertRapidJSON(&json[i], depth + 1));
		}
		return array;
	}
	if (json->IsObject()) {
		boost::shared_ptr<JSONObject> object = boost::make_shared<JSONObject>();
		rapidjson::Value::ConstMemberIterator it = json->MemberBegin();
		for (; it != json->MemberEnd(); it++) {
			JSONValue::ref childValue = convertRapidJSON(&it->value, depth + 1);
			if (!childValue) {
				/* Let the failure bubble up */
				return boost::shared_ptr<JSONValue>();
			}
			object->set(it->name.GetString(), childValue);
		}
		return object;
	}
	/* Fallthrough */
	return boost::shared_ptr<JSONValue>();
}

boost::shared_ptr<JSONObject> JSONObject::parse(const std::string& source) {
	rapidjson::Document d;
	d.Parse(source.c_str());

	return boost::dynamic_pointer_cast<JSONObject>(convertRapidJSON(&d, 0));
}

}
