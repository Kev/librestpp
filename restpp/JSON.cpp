/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#include "JSON.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace librestpp {

JSONValue::~JSONValue() {

}

JSONNumber::JSONNumber(int64_t value) : int64Value_(value) {

}

JSONNumber::JSONNumber(uint64_t value) : uint64Value_(value) {

}

JSONNumber::JSONNumber(double value) : doubleValue_(value) {

}


int64_t JSONNumber::getInt64Value() {
	return int64Value_.get();
}

uint64_t JSONNumber::getUInt64Value() {
	return uint64Value_.get();
}

double JSONNumber::getDoubleValue() {
	return doubleValue_.get();
}

bool JSONNumber::canBeInt64() {
	//FIXME: conversions
	return !!int64Value_;
}

bool JSONNumber::canBeUInt64() {
	//FIXME: conversions
	return !!uint64Value_;
}

bool JSONNumber::canBeDouble() {
	//FIXME: conversions
	return !!doubleValue_;
}


JSONValue::ref JSONNumber::set(int64_t value) {
	int64Value_ = value;
	return shared_from_this();
}

JSONValue::ref JSONNumber::set(uint64_t value) {
	uint64Value_ = value;
	return shared_from_this();
}

JSONValue::ref JSONNumber::set(double value) {
	doubleValue_ = value;
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

JSONValue::ref JSONObject::set(const std::string& key, const std::string& value) {
	return set(key, std::make_shared<JSONString>(value));
}

JSONValue::ref JSONObject::set(const std::string& key, JSONValue::ref value) {
	values_[key] = value;
	return shared_from_this();
}

std::map<std::string, JSONValue::ref> JSONObject::getValues() {
	return values_;
}

void jsonValueToRapidJSON(JSONValue* value, rapidjson::Value& rapidValue, rapidjson::Document& document) {
	JSONNull* nullValue = dynamic_cast<JSONNull*>(value);
	if (nullValue) {
		rapidValue.SetNull();
		return;
	}
	JSONNumber* numberValue = dynamic_cast<JSONNumber*>(value);
	if (numberValue) {
		if (numberValue->canBeUInt64()) {
			rapidValue.SetUint64(numberValue->getUInt64Value());
		}
		else if (numberValue->canBeInt64()) {
			rapidValue.SetInt64(numberValue->getInt64Value());
		}
		else  {
			rapidValue.SetDouble(numberValue->getDoubleValue());
		}
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
		for (auto & value : values) {
			rapidjson::Value obj;
			jsonValueToRapidJSON(value.get(), obj, document);
			rapidValue.PushBack(obj, document.GetAllocator());
		}
		return;
	}
	JSONObject* objectValue = dynamic_cast<JSONObject*>(value);
	if (objectValue) {
		rapidValue.SetObject();
		typedef std::map<std::string, JSONValue::ref> ValuesMap;
		ValuesMap values = objectValue->getValues();
		for (auto & value : values) {
			rapidjson::Value obj;
			jsonValueToRapidJSON(value.second.get(), obj, document);
			rapidjson::Value key;
			key.SetString(value.first.c_str(), value.first.size(), document.GetAllocator());
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

std::string JSONArray::serialize() {
	rapidjson::Document d;
	jsonValueToRapidJSON(this, d, d);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	return buffer.GetString();
}

std::shared_ptr<JSONValue> convertRapidJSON(const rapidjson::Value* json, size_t depth) {
	if (depth > 20) {
		/* stack overflow attack*/
		return std::shared_ptr<JSONValue>();
	}
	if (json->IsNull()) {
		return std::make_shared<JSONNull>();
	}
	if (json->IsBool()) {
		return std::make_shared<JSONBool>(json->GetBool());
	}
	if (json->IsInt64()) {
		return std::make_shared<JSONNumber>(json->GetInt64());
	}
	if (json->IsUint64()) {
		return std::make_shared<JSONNumber>(json->GetUint64());
	}
	if (json->IsDouble()) {
		return std::make_shared<JSONNumber>(json->GetDouble());
	}
	if (json->IsString()) {
		return std::make_shared<JSONString>(json->GetString());
	}
	if (json->IsArray()) {
		std::shared_ptr<JSONArray> array = std::make_shared<JSONArray>();
		for (size_t i = 0; i < json->Size(); i++) {
			array->append(convertRapidJSON(&json[i], depth + 1));
		}
		return array;
	}
	if (json->IsObject()) {
		std::shared_ptr<JSONObject> object = std::make_shared<JSONObject>();
		rapidjson::Value::ConstMemberIterator it = json->MemberBegin();
		for (; it != json->MemberEnd(); it++) {
			JSONValue::ref childValue = convertRapidJSON(&it->value, depth + 1);
			if (!childValue) {
				/* Let the failure bubble up */
				return std::shared_ptr<JSONValue>();
			}
			object->set(it->name.GetString(), childValue);
		}
		return object;
	}
	/* Fallthrough */
	return std::shared_ptr<JSONValue>();
}

std::shared_ptr<JSONObject> JSONObject::parse(const std::string& source) {
	rapidjson::Document d;
	d.Parse(source.c_str());

	return std::dynamic_pointer_cast<JSONObject>(convertRapidJSON(&d, 0));
}

}
