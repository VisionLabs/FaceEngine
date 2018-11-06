#include <pybind11/pybind11.h>
#include <fsdk/FaceEngine.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>
#include "ErrorsAdapter.hpp"
#include "FaceEngineAdapter.hpp"

namespace py = pybind11;

void index_module(py::module& f) {
	// Index
	py::class_<fsdk::IIndexPtr>(f, "IIndexPtr", "Some data structure optimized for search queries.\n")
		.def("search", [](
			const fsdk::IIndexPtr& indexPtr,
			const fsdk::IDescriptorPtr reference,
			const int maxResultsCount) {
				std::vector<fsdk::SearchResult> searchResults(maxResultsCount);
				fsdk::ResultValue<fsdk::FSDKError, int> err = indexPtr->search(
					reference,
					maxResultsCount,
					searchResults.data());
				if (err.isOk()) {
					const uint32_t size = err.getValue();
					searchResults.resize(size);
					return std::make_tuple(FSDKErrorResult(err), std::move(searchResults));
				} else {
					return std::make_tuple(FSDKErrorResult(err), std::vector<fsdk::SearchResult>());
				}
				},
			"Search for descriptors with the shorter distance to passed descriptor.\n"
			"\tArgs:\n"
			"\t\tparam1 (IDescriptorPtr): Descriptor to match against index.\n"
			"\t\tparam2 (int): Maximum count of results. It is upper bound value, it\n"
			"\t\t\tdoes not guarantee to return exactly this amount of results.\n"
			"\tReturns:\n"
			"\t\t(tuple of FSDKErrorResult and list of SearchResults): \n"
			"\t\t\ttuple with FSDKErrorResult and list of SearchResults\n")
				;
	
	py::class_<fsdk::IDenseIndexPtr>(f, "IDenseIndexPtr", "Dense (read only) index.\n")
	
		.def("search", [](
			const fsdk::IDenseIndexPtr& indexPtr,
			const fsdk::IDescriptorPtr reference,
			const int maxResultsCount) {
				std::vector<fsdk::SearchResult> searchResults(maxResultsCount);
				fsdk::ResultValue<fsdk::FSDKError, int> err = indexPtr->search(
				reference,
				maxResultsCount,
				searchResults.data());
					if (err.isOk()) {
						uint32_t size = err.getValue();
						searchResults.resize(size);
						return std::make_tuple(FSDKErrorResult(err), std::move(searchResults));
					} else {
						return std::make_tuple(FSDKErrorResult(err), std::vector<fsdk::SearchResult>());
					}
				},
			"Search for descriptors with the shorter distance to passed descriptor.\n"
			"\tArgs:\n"
			"\t\tparam1 (IDescriptorPtr): Descriptor to match against index.\n"
			"\t\tparam2 (int): Maximum count of results. It is upper bound value, it\n"
			"\t\tdoes not guarantee to return exactly this amount of results.\n"
			"\tReturns:\n"
			"\t\t(tuple of FSDKErrorResult and list): tuple with FSDKErrorResult and list of SearchResults,\n")
		
		.def("size", [](
			const fsdk::IDenseIndexPtr& indexPtr) {
			return indexPtr->size();
		}, "Return size of internal storage. if not initialized, 0 is returned.\n")
		
		.def("descriptorByIndex", [](
			const fsdk::IDenseIndexPtr& indexPtr,
			const fsdk::DescriptorId index,
			const fsdk::IDescriptorPtr& descriptorPtr) {
				fsdk::Result<fsdk::FSDKError> err = indexPtr->descriptorByIndex(index, descriptorPtr);
				if (err.isOk())
					return std::make_tuple(FSDKErrorResult(err), descriptorPtr);
				else
					return std::make_tuple(FSDKErrorResult(err), fsdk::IDescriptorPtr());
			},
			"Requests descriptor data out of internal storage.\n"
			"\t\tparam1 (index): Identification value of some descriptor. Might be received either\n"
			"\t\t\tby using append() methods, or as output of search query. Must be less than size().\n"
			"\t\tparam2 (descriptor): created descriptor object with correctly set\n"
			"\t\t\tversion and length. Only changes data of passed descriptor.\n"
			"\tReturns:\n"
			"\t\t(tuple of FSDKErrorResult and descriptor):tuple with FSDKErrorResult and descriptor\n"
			"\t\t\tMore detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")
			; // IDenseIndexPtr
		
	py::class_<fsdk::IDynamicIndexPtr>(f, "IDynamicIndexPtr", "Dynamic index")
		
		.def("saveToDenseIndex", [](const fsdk::IDynamicIndexPtr& dynamicIndex, const char* path) {
			fsdk::Result<fsdk::FSDKError> err = dynamicIndex->saveToDenseIndex(path);
			return FSDKErrorResult(err);
		}, "Saves index as dense. To load saved index use @see loadDenseIndex method.\n"
			"\tDense index cannot be loaded as dynamic.\n"
			"\tArgs:\n"
			"\t\tparam1 (str): Path to file to be created and filled with index data.\n"
			"\t\tAny extension is acceptable.\n"
			"\tReturns:\n"
			"\t\t(FSDKErrorResult): One of the error codes specified by FSDKErrorResult\n")
		
		.def("saveToDynamicIndex", [](const fsdk::IDynamicIndexPtr& indexPtr, const char* path) {
			fsdk::Result<fsdk::FSDKError> err = indexPtr->saveToDynamicIndex(path);
			return FSDKErrorResult(err);
		}, "Saves index as dynamic. To load saved index use @see loadDynamicIndex method.\n"
			"\tDynamic index cannot be loaded as dense.\n"
			"\tArgs:\n"
			"\t\tparam1 (str): Path to file to be created and filled with index data.\n"
			"\t\tAny extension is acceptable.\n"
			"\tReturns:\n"
			"\t\t(FSDKErrorResult): One of the error codes specified by FSDKErrorResult\n")
		
		.def("countOfIndexedDescriptors", [](const fsdk::IDynamicIndexPtr& dynamicIndex){
			return dynamicIndex->countOfIndexedDescriptors();
		}, "Returns count of indexed descriptors.\n"
			"\t\t You may wonder why this method exists if we have size() method. The reason is that\n"
			"\t\t removeDescriptor behaves differently on\n"
			"\t\t IIndexBuild and IDynamicIndex. On builder it does actually erases\n"
			"\t\t descriptor out of internal storage, but it does not erase it if used on\n"
			"\t\t IDynamicIndex. The reason is that graph data structure relies on indexes\n"
			"\t\t being constant, so removeDescriptor only removes it out of graph, so it is not\n"
			"\t\t discoverable see search. So this methods returns actuall data\n"
			"\t\t storage size minus count of removed descriptors.\n"
			"\tReturns:\n"
			"\t\t(int): Count of indexed descriptors.\n"
			"\t\t\tMore detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")
		
		.def("search", [](
			const fsdk::IDynamicIndexPtr& indexPtr,
			const fsdk::IDescriptorPtr reference,
			const int maxResultsCount) {
				std::vector<fsdk::SearchResult> searchResults(maxResultsCount);
				fsdk::ResultValue<fsdk::FSDKError, int> err = indexPtr->search(
					reference,
					maxResultsCount,
					searchResults.data());
				if (err.isOk()) {
					uint32_t size = err.getValue();
					searchResults.resize(size);
					return std::make_tuple(FSDKErrorResult(err), std::move(searchResults));
				} else {
					return std::make_tuple(FSDKErrorResult(err), std::vector<fsdk::SearchResult>());
				}
			},
			"Search for descriptors with the shorter distance to passed descriptor.\n"
			"\tArgs:\n"
			"\t\tparam1 (IDescriptorPtr): Descriptor to match against index.\n"
			"\t\tparam2 (int): Maximum count of results. It is upper bound value, it\n"
			"\t\t\tdoes not guarantee to return exactly this amount of results.\n"
			"\tReturns:\n"
			"\t\t(tuple of FSDKErrorResult and list): \n"
			"\t\t\ttuple with FSDKErrorResult and list of SearchResults\n")
		
		.def("size", [](
			const fsdk::IDynamicIndexPtr& indexPtr) {
			return indexPtr->size();
		}, "Returns size of internal storage. If not initialized, 0 is returned.\n")
		
		.def("descriptorByIndex", [](
			const fsdk::IDynamicIndexPtr& indexPtr,
			const fsdk::DescriptorId index,
			const fsdk::IDescriptorPtr& descriptorPtr) {
				fsdk::Result<fsdk::FSDKError> err = indexPtr->descriptorByIndex(index, descriptorPtr);
				return std::make_tuple(FSDKErrorResult(err), descriptorPtr);
			},
			"Requests descriptor data out of internal storage.\n"
			"\t\tparam1 (index): Identification value of some descriptor. Might be received either\n"
			"\t\t\tby using append methods, or as output of search query. Must be less than size().\n"
			"\t\tparam2 (descriptor): created descriptor object with correctly set\n"
			"\t\tversion and length. Only changes data of passed descriptor.\n"
			"\tReturns:\n"
			"\t\t(tuple with FSDKErrorResult and descriptor): tuple with FSDKErrorResult and descriptor\n"
			"\t\t\tMore detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")
		
		.def("appendDescriptor", [](const fsdk::IDynamicIndexPtr& index, const fsdk::IDescriptorPtr batch) {
				fsdk::ResultValue<fsdk::FSDKError, fsdk::DescriptorId> err = index->appendDescriptor(batch);
				return FSDKErrorValueInt(err);
			},
			"Appends descriptor to internal storage.\n"
			"\t\tparam1 (descriptor): created descriptor with correct length, version and data\n"
			"\tReturns:\n"
			"\t\t(FSDKErrorValueInt): One of the error codes specified by FSDKError and identification\n"
			"\t\t\tof appended descriptor. Such identification might be used to query descriptor\n"
			"\t\t\twith descriptorByIndex() or remove it from storage\n"
			"\t\t\twith removeDescriptor()\n"
			"\t\t\tMore detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")
		
		.def("appendBatch", [](
			const fsdk::IDynamicIndexPtr& index,
			const fsdk::IDescriptorBatchPtr batch) {
				fsdk::ResultValue<fsdk::FSDKError, fsdk::DescriptorId> err = index->appendBatch(batch);
				return FSDKErrorValueInt(err);
			},
			"Appends batch of descriptors to internal storage.\n"
			"\t\tparam1 (batch): Batch of descriptors with correct length, version and data\n"
			"\tReturns:\n"
			"\t\t(FSDKErrorValueInt): One of the error codes specified by FSDKError and identification\n"
			"\t\tof appended descriptor. Other descriptors from batch are appended\n"
			"\t\tsequentially in the same order as they are in the batch. Such identification\n"
			"\t\t might be used to query descriptor with\n"
			"\t\tdescriptorByIndex() or remove it from storage\n"
			"\t\twith removeDescriptor().\n"
			"\t\t\tMore detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")
		
		.def("removeDescriptor", [](const fsdk::IDynamicIndexPtr& dynamicIndex, const fsdk::DescriptorId id){
				fsdk::Result<fsdk::FSDKError> err = dynamicIndex->removeDescriptor(id);
				return FSDKErrorResult(err);
			},
			"Removes descriptor out of internal storage.\n"
			"\t\tIf used on IDynamicIndex graph updates itself too.\n"
			"\t\tIMPORTANT: If used on IDynamicIndex it will NOT actually erase\n"
			"\t\tdescriptor with given index out of internal storage. Instead, it will\n"
			"\t\tremove it out of graph, so it is not searchable.\n"
			"\t\tIf used on @see IIndexBuilder, it WILL actually erase it. But beware:\n"
			"\t\tif your storage is big enough performance might be very poor, because\n"
			"\t\tdescriptors are stored sequentially in vector-like data structure, so every\n"
			"\t\telement after erased will be moved.\n"
			"\t\tparam1 (index): Identification of descriptors position in internal storage.\n"
			"\t\tIs received by using append methods or search()\n"
			"\tReturns:\n"
			"\t\t(FSDKErrorValueInt): One of the error codes specified by FSDKError\n"
			"\t\t\tMore detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")
			; // IDynamicIndexPtr
	
	py::class_<fsdk::IIndexBuilderPtr>(f, "IIndexBuilderPtr")
	
		.def("buildIndex", [](const fsdk::IIndexBuilderPtr& indexBuilderPtr) {
			auto res = indexBuilderPtr->buildIndex();
			if (res.isOk())
				return std::make_tuple(FSDKErrorResult(fsdk::makeResult(res.getError())), fsdk::acquire(res.getValue()));
			else
				return std::make_tuple(FSDKErrorResult(fsdk::makeResult(res.getError())), fsdk::IDynamicIndexPtr());
		}, "Builds index with every descriptor appended. Blocks until completed.\n"
			"\t\t Is very heavy method in terms of computing load.\n")
		
		.def("appendDescriptor", [](const fsdk::IIndexBuilderPtr& indexBuilderPtr, const fsdk::IDescriptorPtr batch){
				fsdk::ResultValue<fsdk::FSDKError, fsdk::DescriptorId> err = indexBuilderPtr->appendDescriptor(batch);
				return FSDKErrorValueInt(err);
			},
			"Appends descriptor to internal storage.\n"
			"\t\tparam1 (descriptor): created descriptor with correct length, version and data\n"
			"\tReturns:\n"
			"\t\t(FSDKErrorValueInt): One of the error codes specified by FSDKError and identification\n"
			"\t\t\tof appended descriptor. Such identification might be used to query descriptor\n"
			"\t\t\twith descriptorByIndex() or remove it from storage\n"
			"\t\t\twith removeDescriptor()\n"
			"\t\t\tMore detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")
		
		.def("appendBatch", [](
			const fsdk::IIndexBuilderPtr& indexBuilderPtr,
			const fsdk::IDescriptorBatchPtr batch) {
				fsdk::ResultValue<fsdk::FSDKError, fsdk::DescriptorId> err = indexBuilderPtr->appendBatch(batch);
				return FSDKErrorValueInt(err);
			},
			"Appends batch of descriptors to internal storage.\n"
			"\t\tparam1 (batch): Batch of descriptors with correct length, version and data\n"
			"\tReturns:\n"
			"\t\t(FSDKErrorValueInt): One of the error codes specified by FSDKError and identification\n"
			"\t\t\tof appended descriptor. Other descriptors from batch are appended\n"
			"\t\t\tsequentially in the same order as they are in the batch. Such identification\n"
			"\t\t\tmight be used to query descriptor with\n"
			"\t\t\tdescriptorByIndex() or remove it from storage\n"
			"\t\t\twith removeDescriptor().\n"
			"\t\t\tMore detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")
		
		.def("removeDescriptor", [](const fsdk::IIndexBuilderPtr& indexBuilderPtr, const fsdk::DescriptorId index) {
				fsdk::Result<fsdk::FSDKError> err = indexBuilderPtr->removeDescriptor(index);
				return FSDKErrorResult(err);
			},
			"Removes descriptor out of internal storage.\n"
			"If used on IDynamicIndex graph updates itself too.\n"
			"\t\tIMPORTANT: If used on IDynamicIndex it will NOT actually erase\n"
			"\t\tdescriptor with given index out of internal storage. Instead, it will\n"
			"\t\tremove it out of graph, so it is not searchable.\n"
			"\t\tIf used on @see IIndexBuilder, it WILL actually erase it. But beware:\n"
			"\t\tif your storage is big enough performance might be very poor, because\n"
			"\t\tdescriptors are stored sequentially in vector-like data structure, so every\n"
			"\t\telement after erased will be moved.\n"
			"\t\tparam1 (index): Identification of descriptors position in internal storage.\n"
			"\t\tIs received by using append methods or search()\n"
			"\tReturns:\n"
			"\t\t(FSDKErrorValueInt): One of the error codes specified by FSDKError"
			"\t\tMore detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")
		
		.def("descriptorByIndex", [](
			const fsdk::IIndexBuilderPtr& indexBuilderPtr,
			const fsdk::DescriptorId index,
			const fsdk::IDescriptorPtr& descriptorPtr) {
				fsdk::Result<fsdk::FSDKError> err = indexBuilderPtr->descriptorByIndex(index, descriptorPtr);
			
				if (err.isOk())
					return std::make_tuple(FSDKErrorResult(err), descriptorPtr);
				else
					return std::make_tuple(FSDKErrorResult(err), fsdk::IDescriptorPtr());
			},
			"Requests descriptor data out of internal storage.\n"
			"\t\tparam1 (index): Identification value of some descriptor. Might be received either\n"
			"\t\t\tby using append methods, or as output of search query. Must be less than size().\n"
			"\t\tparam2 (descriptor): created descriptor object with correctly set\n"
			"\t\tversion and length. Only changes data of passed descriptor.\n"
			"\tReturns:\n"
			"\t\t(tuple of FSDKErrorResult and descriptor):tuple with FSDKErrorResult and descriptor\n"
			"\t\tMore detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")
				;
}
