#include <pybind11/pybind11.h>
#include <fsdk/FaceEngine.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>
#include "ErrorsAdapter.hpp"
#include "helpers.hpp"

namespace py = pybind11;

void descriptor_module(py::module& f) {
// descriptor
py::class_<fsdk::IDescriptorPtr>(f, "IDescriptorPtr", "Descriptor interface. Used for matching.\n")
	
	.def("getModelVersion",[]( const fsdk::IDescriptorPtr& desc) {
			return desc->getModelVersion(); },
		"Get algorithm model version this descriptor was created with.\n"
		"\tReturns:\n"
		"\t\t(int): Version as integral number.\n")
	
	.def("getDescriptorLength",[]( const fsdk::IDescriptorPtr& desc) {
			return desc->getDescriptorLength(); },
		"Return size of descriptor in bytes.\n"
		"This method is thread safe.\n"
		"\tReturns:\n"
		"\t\t(int): size of descriptor in bytes.\n")
	
	.def("getDescriptor",[]( const fsdk::IDescriptorPtr& desc) {
			const uint32_t size = desc->getDescriptorLength();
			std::vector<uint8_t> buffer(size);
			bool isOk = desc->getDescriptor(buffer.data());
			if (isOk)
				return std::move(buffer);
			else
				return std::vector<uint8_t>(); },
		"Copy descriptor data to python list.\n "
		"\tThis method is thread safe"
		"\tReturns:\n"
		"\t\t(list): list of uint8_t if is ok, empty list if ERROR")
	.def("getData",[]( const fsdk::IDescriptorPtr& desc) {
			 const uint32_t size = desc->getDescriptorLength();
			 std::vector<uint8_t> buffer(size);
			 bool isOk = desc->getDescriptor(buffer.data());
			 if (isOk)
				 return py::bytes((const char*)buffer.data(), buffer.size());
			 else
				 return py::bytes(); },
		 "Returns descriptor as bytes.\n")
	.def("load",[](
		const fsdk::IDescriptorPtr& descriptor,
		const char* buffer,
		uint32_t bufferSize) {
			VectorArchive archiveDescriptor(buffer, bufferSize);
			fsdk::Result<fsdk::ISerializableObject::Error> err = descriptor->load(&archiveDescriptor);
			return SerializeErrorResult(err);
		}, "Load descriptor from buffer")
			; // descriptor
	
// DescriptorBatch
py::class_<fsdk::IDescriptorBatchPtr>(f, "IDescriptorBatchPtr", "Descriptor batch interface. "
	"Used for matching large continuous sets of descriptors")
	
	.def("add",[] (
		const fsdk::IDescriptorBatchPtr& descriptorBatchPtr,
		const fsdk::IDescriptorPtr& desc) {
			fsdk::Result<fsdk::IDescriptorBatch::Error> error = descriptorBatchPtr->add(desc);
			return DescriptorBatchResult(error);
		},
		"Add a descriptor to the batch.\n"
		"\tWhen adding the first descriptor to an empty batch, the initial internal parameters (like version,\n"
		"\tetc) of the batch are set to correspond ones of the descriptor. This means that incompatible descriptors\n"
		"\tadded later will be rejected. See getModelVersion() to check whether a descriptor can be added to the batch.\n"
		"\tArgs:\n"
		"\t\tparam1 (IDescriptorPtr): descriptor to add. Descriptor data is copied and to internal reference is held,\n"
		"\t\tthus it is safe to release the source descriptor object later.\n"
		"\tReturns:\n"
		"\t\t(DescriptorBatchResult): One of the error codes specified by DescriptorBatchError.\n")
	
	.def("removeFast",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr, const int index) {
			fsdk::Result<fsdk::IDescriptorBatch::Error> error = descriptorBatchPtr->removeFast(index);
			return DescriptorBatchResult(error); },
		"Remove a descriptor from batch. \nRemove descriptor by swapping it with the last descriptor in batch.\n"
		"\tThis preserves descriptor order.\n"
		"\tArgs:\n"
		"\t\tparam1 (int): descriptor index\n"
		"\tReturns:\n"
		"\t\t(DescriptorBatchResult): One of the error codes specified by DescriptorBatchError.\n")
	
	.def("removeSlow",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr, int index) {
			fsdk::Result<fsdk::IDescriptorBatch::Error> error = descriptorBatchPtr->removeSlow(index);
			return  DescriptorBatchResult(error);},
		"Remove a descriptor from batch.\n"
		"\tRemove descriptor by shifting all the following descriptors back. This preserves descriptor order.\n"
		"\tArgs:\n"
		"\t\tparam1 (int): descriptor index\n"
		"\tReturns:\n"
		"\t\t(DescriptorBatchResult): One of the error codes specified by DescriptorBatchError.\n" )
	
	.def("getMaxCount",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr) {
			return descriptorBatchPtr->getMaxCount(); },
		"Get maximum number of descriptors in this batch.\n"
		"\tReturns:\n"
		"\t\t(int): maximum number of descriptors in this batch.\n")
	
	.def("getCount",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr) {
			return descriptorBatchPtr->getCount(); },
		"Get actual number of descriptors in this batch.\n"
		"\tReturns:\n"
		"\t\t(int):actual number of descriptors in this batch.\n")
	
	.def("getModelVersion",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr) {
			return descriptorBatchPtr->getModelVersion(); },
		"Get algorithm model version the descriptors in this batch were created with.\n"
		"This function only makes sense when there is at least one descriptor in the batch. It will return 0 if\n"
		"the batch is empty.\n"
		"\tReturns:\n"
		"\t\t(int): Version as integral number.\n")
	
	.def("getDescriptorSize",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr) {
			return descriptorBatchPtr->getDescriptorSize(); },
		"Get length of one descriptor. Specified by version of descriptors in batch.\n"
		"\tReturns:\n"
		"\t\t(int): Length of one descriptor in batch.\n")
	
	.def("getDescriptorSlow",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr, int index) {
			return fsdk::acquire(descriptorBatchPtr->getDescriptorSlow(index)); },
		"Create descriptor from batch by index with copying\n"
		"\tArgs:\n"
		"\t\tparam1 (int):  index required descriptor in batch\n"
		"\tReturns:\n"
		"\t\t(IDescriptorPtr): valid object if succeeded.\n")
	
	.def("getDescriptorFast",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr, int index) {
			return fsdk::acquire(descriptorBatchPtr->getDescriptorFast(index)); },
		"Create descriptor from batch by index without copying\n"
		"\tArgs:\n"
		"\t\tparam1 (int): index required descriptor in batch\n"
		"\tReturns:\n"
		"\t\t(IDescriptorPtr): valid object if succeeded.\n")
	
	.def("load",[](
		const fsdk::IDescriptorBatchPtr& descriptorBatchPtr,
			const char* buffer,
			uint32_t bufferSize) {
				VectorArchive archiveDescriptor(buffer, bufferSize);
				fsdk::Result<fsdk::ISerializableObject::Error> err =
					descriptorBatchPtr->load(&archiveDescriptor, bufferSize);
				return SerializeErrorResult(err);
		
			}, "Load descriptor from buffer")
			;
	
	py::class_<fsdk::IDescriptorExtractorPtr>(f, "IDescriptorExtractorPtr",
		"Descriptor extractor interface.\n"
		"\tExtracts face descriptors from images. The descriptors can be later used for face matching.\n")
	
	.def("extract",[](
		const fsdk::IDescriptorExtractorPtr& extractor,
		fsdk::Image& image,
		const fsdk::BaseDetection<float>& detection,
		const fsdk::Landmarks5& landmarks,
		const fsdk::IDescriptorPtr& descriptor) {
			fsdk::ResultValue<fsdk::FSDKError, float> err = extractor->extract(
				image,
				detection,
				landmarks,
				descriptor);
			return FSDKErrorValueFloat(err); },
		"Extract a face descriptor from an image.\n"
		"\tThis method accepts arbitrary images that have size at least 250x250 pixels and R8G8B8 pixel format.\n"
		"\tThe input image is warped internally using an assigned warper (@see IWarper). The descriptor extractor is\n"
		"\tcreated with a proper warped assigned by default so no additional setup is required, unless this behaviour\n"
		"\tis overriden with `defaultWarper` flag upon extractor creation.\n"
		"\tArgs:\n"
		"\t\tparam1 (Image):  source image. Format must be R8G8B8\n"
		"\t\tparam2 (Detection): face detection\n"
		"\t\tparam3 (Landmarks5): face feature set\n"
		"\t\tparam4 (IDescriptorPtr) [out]: descriptor to fill with data.\n"
		"\tReturns:\n"
		"\t\t(FSDKErrorValueFloat): ResultValue with error code specified by FSDKError and score of descriptor normalized in range [0, 1]\n"
		"\t\t\t1 - face on the input warp; 0 - garbage on the input detection.\n"
		"\t\t\tScore is fake if extractor uses mobile net version of extraction model.\n"
		"\t\tSee FSDKErrorValueFloat.\n")
	
	.def("extractFromWarpedImage",[](
		const fsdk::IDescriptorExtractorPtr& extractor,
		const fsdk::Image& image,
		const fsdk::IDescriptorPtr& descriptor) {
			fsdk::ResultValue<fsdk::FSDKError, float> err = extractor->extractFromWarpedImage(image, descriptor);
			return FSDKErrorValueFloat(err); },
		"Extract descriptor from a warped image.\n"
		"\tThe input image should be warped; see IWarper.\n"
		"\tArgs:\n"
		"\t\tparam1 (Image): image source warped image, should be a valid 250x250 image in R8G8B8 format.\n"
		"\t\tparam2 (IDescriptorPtr) [out]: descriptor to fill with data\n"
		"\tReturns:\n"
		"\t\t(FSDKErrorValueFloat): ResultValue with error code specified by FSDKError and score of descriptor normalized in range [0, 1]\n"
		"\t\t\t1 - face on the input warp; 0 - garbage on the input detection.\n"
		"\t\t\tScore is fake if extractor uses mobile net version of extraction model.\n"
		"\t\tSee FSDKErrorValueFloat.\n")
	
	.def("extractFromWarpedImageBatch",[](
		const fsdk::IDescriptorExtractorPtr& extractor,
		const std::vector<fsdk::Image>& warpsBatch,
		const fsdk::IDescriptorBatchPtr& descriptorBatch,
		const fsdk::IDescriptorPtr& aggregation,
		uint32_t batchSize) {
			std::vector<float> garbageScoreBatch(batchSize);
			fsdk::ResultValue<fsdk::FSDKError, float> err = extractor->extractFromWarpedImageBatch(
				warpsBatch.data(),
				descriptorBatch,
				aggregation,
				garbageScoreBatch.data(),
				batchSize);
			if (err.isOk()) {
				return std::make_tuple(FSDKErrorValueFloat(err), std::move(garbageScoreBatch));
			} else
				return std::make_tuple(FSDKErrorValueFloat(err), std::vector<float>());
		},
		"Extract batch of descriptors from a batch of images and perform aggregation.\n"
		"\tThe input images should be warped; see IWarper.\n"
		"\tArgs:\n"
		"\t\tparam1 (list):  input array of warped images, images should be in R8G8B8 format, "
		"with size 250x250\n"
		"\t\tparam2 (IDescriptorBatchPtr) [out]:  descriptorBatch descriptor batch to fill with data.\n"
		"\t\tparam3 (IDescriptorPtr) [out]: descriptor with aggregation based on descriptor batch.\n"
		"\t\tparam4 (int): size of the batch.\n"
		"\tReturns:\n"
		"\t\t(list): if OK - list of descriptor scores normalized in range [0, 1]\n"
		"\t\t\t1 - face on the input warp; 0 - garbage on the input warp.\n"
		"\t\t\tScore is fake if extractor uses mobile net version of extraction model.\n"
		"\t\t(FSDKErrorResult): else - result with error code specified by FSDKError\n See FSDKErrorResult.\n")
	
	.def("extractFromWarpedImageBatch",[](
		const fsdk::IDescriptorExtractorPtr& extractor,
		const std::vector<fsdk::Image>& warpsBatch,
		const fsdk::IDescriptorBatchPtr& descriptorBatch,
		uint32_t batchSize) {
			std::vector<float> garbageScoreBatch(batchSize);
			fsdk::Result<fsdk::FSDKError> err = extractor->extractFromWarpedImageBatch(
				warpsBatch.data(),
				descriptorBatch,
				garbageScoreBatch.data(),
				batchSize);
			if (err.isOk())
				return std::make_tuple(FSDKErrorResult(err), std::move(garbageScoreBatch));
			else
				return std::make_tuple(FSDKErrorResult(err), std::vector<float>());
		},
		"Extract batch of descriptors from a batch of images.\n"
		"\tThe input images should be warped; see IWarperPtr.\n"
		"\tArgs:\n"
		"\t\tparam1 (list): input list of warped images.Images should be in R8G8B8 format , with size 250x250\n"
		"\t\tparam2 (IDescriptorBatchPtr) [out]: \n"
		"\t\tparam3 (int): size of the batch\n"
		"\tReturns:\n"
		"\t\t(list): if OK - list of descriptor scores normalized in range [0, 1]\n"
		"\t\t\t1 - face on the input warp; 0 - garbage on the input warp.\n"
		"\t\t\t Score is fake if extractor uses mobile net version of extraction model.\n"
		"\t\t(FSDKErrorResult): else - result with error code specified by FSDKError\n See FSDKErrorResult.\n")
	;
	
	py::class_<fsdk::IDescriptorMatcherPtr>(f, "IDescriptorMatcherPtr",
		"Descriptor matcher interface.\n"
		"\tMatches descriptors 1:1 and 1:M (@see IDescriptor and IDescriptorBatch interfaces).\n"
		"\tAs a result of the matching process the calling site gets a MatchingResult "
		"(or several of them in case of 1:M\n"
		"\tmatching). The MatchingResult structure contains distance and similarity metrics.\n"
		"\t\n"
		"\tDistance is measured in abstract units and tends to 0 for similar descriptors "
		"and to infinity for different ones.\n"
		"\tSimilarity is the opposite metric and shows probability of two "
		"descriptors belonging to the same person; therfore\n"
		"\tit is normalized to [0..1] range")
	
	.def("match",[](
		const fsdk::IDescriptorMatcherPtr& matcherPtr,
		const fsdk::IDescriptorPtr& first,
		const fsdk::IDescriptorPtr& second) {
			fsdk::ResultValue<fsdk::FSDKError, fsdk::MatchingResult> err =
				matcherPtr->match(first, second);
			return FSDKErrorValueMatching(err); },
		"Match descriptors 1:1.\n"
		"\tArgs\n"
		"\t\tparam1 (IDescriptorPtr): first descriptor\n"
		"\t\tparam2 (IDescriptorPtr): second descriptor\n"
		"\tReturns:\n"
		"\t\t(FSDKErrorValueMatching): Value with error code specified by FSDKError and matching result. "
		"See FSDKErrorValueMatching.\n")
	
	.def("match",[](
		const fsdk::IDescriptorMatcherPtr& matcherPtr,
		const fsdk::IDescriptorPtr& reference,
		const fsdk::IDescriptorBatchPtr& candidates) {
			std::vector<fsdk::MatchingResult> results(candidates->getCount());
			fsdk::Result<fsdk::FSDKError> err =
			matcherPtr->match(reference, candidates, results.data());
			if (err.isOk()) {
				return std::make_tuple(FSDKErrorResult(err), std::move(results));
			}
			else
				return std::make_tuple(FSDKErrorResult(err), std::vector<fsdk::MatchingResult>());
		},
		"Match descriptors 1:M.\n"
		"\tMatches a reference descriptor to a batch of candidate descriptors. "
		"The results are layed out in the\n"
		"\tsame order as the candidate descriptors in the batch.\n"
		"\tArgs\n"
		"\t\tparam1 (IDescriptorPtr): the reference descriptor\n"
		"\t\tparam2 (IDescriptorPtr): the candidate descriptor batch to match with the reference\n"
		"\tReturns:\n"
		"\t\t(list): if OK - matchig result list.\n"
		"\t\t\tLength of `results` must be at least the same as the length of the candidates batch.\n"
		"\t\t\tIDescriptorBatchPtr::getMaxCount()\n"
		"\t\t(FSDKErrorResult wrapped in list): else - result with error specified by FSDKErrorResult.\n")
	;
}
