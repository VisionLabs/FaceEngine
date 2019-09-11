//
// Created by mar on 27.04.18.
//
#pragma once

#include <fsdk/FaceEngine.h>
#include <lsdk/LivenessEngine.h>
#include <fstream>

struct FSDKErrorResult {
	bool isOk;
	bool isError;
	fsdk::FSDKError error;
	const char* what;

	FSDKErrorResult(fsdk::Result<fsdk::FSDKError> err) :
	isOk(err.isOk()),
	isError(err.isError()),
	error(err.getError()),
	what(err.what())
	{};
};

// liveness error
struct LSDKErrorResult {
	bool isOk;
	bool isError;
	lsdk::LSDKError error;
	const char* what;
	
	LSDKErrorResult(fsdk::ResultValue<lsdk::LSDKError, bool> err) :
		isOk(err.isOk()),
		isError(err.isError()),
		error(err.getError()),
		what(err.what())
	{};
};


struct SerializeErrorResult {
	bool isOk;
	bool isError;
	fsdk::ISerializableObject::Error error;
	const char* what;
	
	SerializeErrorResult(fsdk::Result<fsdk::ISerializableObject::Error> err) :
	isOk(err.isOk()),
	isError(err.isError()),
	error(err.getError()),
	what(err.what())
	{};
};

struct ImageErrorResult {
	bool isOk;
	bool isError;
	fsdk::Image::Error error;
	const char* what;

	ImageErrorResult(fsdk::Result<fsdk::Image::Error> err) :
	isOk(err.isOk()),
	isError(err.isError()),
	error(err.getError()),
	what(err.what())
	{};
};

struct DescriptorBatchResult {
	bool isOk;
	bool isError;
	fsdk::IDescriptorBatch::Error error;
	const char* what;

	DescriptorBatchResult(fsdk::Result<fsdk::IDescriptorBatch::Error> err) :
	isOk(err.isOk()),
	isError(err.isError()),
	error(err.getError()),
	what(err.what())
	{};
};

struct FSDKErrorValueInt {
	bool isOk;
	bool isError;
	fsdk::FSDKError error;
	const char* what;
	uint32_t value;

	FSDKErrorValueInt(fsdk::ResultValue<fsdk::FSDKError, int> err) :
	isOk(err.isOk()),
	isError(err.isError()),
	error(err.getError()),
	what(err.what()),
	value(err.isOk() ? err.getValue() : 0)
	{};
	
	FSDKErrorValueInt(fsdk::ResultValue<fsdk::FSDKError, fsdk::DescriptorId> err) :
	isOk(err.isOk()),
	isError(err.isError()),
	error(err.getError()),
	what(err.what()),
	value(err.isOk() ? int(err.getValue()) : 0)
	{};
};

struct FSDKErrorValueBool {
	bool isOk;
	bool isError;
	fsdk::FSDKError error;
	const char* what;
	bool value;
	
	FSDKErrorValueBool(fsdk::ResultValue<fsdk::FSDKError, bool> err) :
		isOk(err.isOk()),
		isError(err.isError()),
		error(err.getError()),
		what(err.what()),
		value(err.isOk() ? err.getValue() : false)
	{};
};

struct FSDKErrorValueFloat {
	bool isOk;
	bool isError;
	fsdk::FSDKError error;
	const char* what;
	float value;

	FSDKErrorValueFloat(fsdk::ResultValue<fsdk::FSDKError, float> err) :
	isOk(err.isOk()),
	isError(err.isError()),
	error(err.getError()),
	what(err.what()),
	value(err.isOk() ? err.getValue() : 0.f)
	{};
};

struct FSDKErrorValueMatching {
	bool isOk;
	bool isError;
	fsdk::FSDKError error;
	const char* what;
	fsdk::MatchingResult value;

	FSDKErrorValueMatching(fsdk::ResultValue<fsdk::FSDKError, fsdk::MatchingResult> err) :
	isOk(err.isOk()),
	isError(err.isError()),
	error(err.getError()),
	what(err.what()),
	value(err.isOk() ? err.getValue() : fsdk::MatchingResult{})
	{};
};

struct SettingsProviderResult {
	bool isOk;
	bool isError;
	fsdk::ISettingsProvider::Error error;
	const char* what;

	SettingsProviderResult(fsdk::Result<fsdk::ISettingsProvider::Error> err) :
	isOk(err.isOk()),
	isError(err.isError()),
	error(err.getError()),
	what(err.what())
	{};
};