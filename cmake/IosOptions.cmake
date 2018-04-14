# /cmake/IosOptions.cmake functions 
# functions are called from root cmake
# it is necessary once use: include(<path to>/cmake/IosOptions.cmake)

function(set_ios_options)
	set(FULL_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH} PARENT_SCOPE)
	# boolean is not recognized for some reason
	option(IOS_BUILD_ONLY_ACTIVE_ARCH "To build for only active architecture, not universal binary for every valid arch" OFF)
	option(IOS_BUILD_WITH_BITCODE "To build iOS targets with bitcode enabled" ON)
	
	if(IOS_BUILD_ONLY_ACTIVE_ARCH)
		set(IOS_ONLY_ACTIVE_ARCH_XCODE_FORMAT YES PARENT_SCOPE)
	else()
		set(IOS_ONLY_ACTIVE_ARCH_XCODE_FORMAT NO PARENT_SCOPE)
	endif()

	if(IOS_BUILD_WITH_BITCODE)
		set(IOS_ENABLE_BITCODE_XCODE_FORMAT YES PARENT_SCOPE)
	else()
		set(IOS_ENABLE_BITCODE_XCODE_FORMAT NO PARENT_SCOPE)
	endif()
endfunction()


# OUTPUT_NAME parameter is optional, if it is empty please pass ""
# example: set_ios_framework_properties(SDK "ru.visionlabs.FaceEngine" ${FULL_VERSION} "fsdk" ${XCODE_PUBLIC_INCLUDES})
# of if OUTPUT_NAME is empty:
# example: set_ios_framework_properties("PhotoMaker" "ru.visionlabs.PhotoMaker" ${FULL_VERSION} "" ${XCODE_PUBLIC_INCLUDES})
# ${XCODE_PUBLIC_INCLUDES} is optional BUT better to pass it!
function(set_ios_framework_properties _TARGET_NAME _BUNDLE_IDENTIFIER _FULL_VERSION _OUT_NAME)
	# setting _FULL_VERSION in internal scope
	set(_HEADERS ${ARGN})
	if(_FULL_VERSION STREQUAL "..")
		set(_FULL_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
	endif()
	if(IOS_BUILD_WITH_BITCODE)
		target_compile_options(${_TARGET_NAME} PRIVATE -fembed-bitcode)
		set_target_properties(${_TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_BITCODE_GENERATION_MODE bitcode)
	endif()
		
	set_target_properties(${_TARGET_NAME} PROPERTIES
		FRAMEWORK TRUE
		# "current version" in semantic format in Mach-O binary file
		VERSION "${_FULL_VERSION}"
		# "compatibility version" in semantic format in Mach-O binary file
		SOVERSION "${_FULL_VERSION}"
		# 3 flags for info.plist:
		MACOSX_FRAMEWORK_BUNDLE_VERSION "${_FULL_VERSION}"
		MACOSX_FRAMEWORK_IDENTIFIER "${_BUNDLE_IDENTIFIER}"
		MACOSX_FRAMEWORK_SHORT_VERSION_STRING "${_FULL_VERSION}"
		XCODE_ATTRIBUTE_ENABLE_BITCODE ${IOS_ENABLE_BITCODE_XCODE_FORMAT}
		## stripping
		XCODE_ATTRIBUTE_STRIP_STYLE "non-global"
		XCODE_ATTRIBUTE_SEPARATE_STRIP YES
		XCODE_ATTRIBUTE_DEPLOYMENT_POSTPROCESSING YES
		# set minimum required version for framework to work
		XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 8.0
		# build lib for every valid arch
		XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH ${IOS_ONLY_ACTIVE_ARCH_XCODE_FORMAT}
		XCODE_ATTRIBUTE_ARCHS "armv7 armv7s arm64"
		XCODE_ATTRIBUTE_VALID_ARCHS "armv7 armv7s arm64"
		XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym"
		PUBLIC_HEADER "${_HEADERS}"
		XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
		# very important, dont delete
		XCODE_ATTRIBUTE_INSTALL_PATH "@rpath"
		XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@loader_path"
		INSTALL_NAME_DIR "@rpath"
		MACOSX_RPATH YES
	)
	
	# output name of framework, optional, 
	if(_OUT_NAME)
		set_target_properties(${_TARGET_NAME} PROPERTIES
		OUTPUT_NAME ${_OUT_NAME})
	endif()
	
endfunction()


function(set_ios_stat_lib_properties STAT_LIB_NAME)
	if(IOS_BUILD_WITH_BITCODE)
		target_compile_options(${STAT_LIB_NAME} PRIVATE -fembed-bitcode)
	endif()
	
	set_target_properties(
		${STAT_LIB_NAME}
		PROPERTIES
		# stripping makes binary smaller. its important for large libraries
		XCODE_ATTRIBUTE_STRIP_STYLE "non-global"
		XCODE_ATTRIBUTE_SEPARATE_STRIP YES
		XCODE_ATTRIBUTE_DEPLOYMENT_POSTPROCESSING YES
		XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 8.0
		# build lib for every valid arch
		XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH ${IOS_ONLY_ACTIVE_ARCH_XCODE_FORMAT}
		XCODE_ATTRIBUTE_ARCHS "armv7 armv7s arm64"
		XCODE_ATTRIBUTE_VALID_ARCHS "armv7 armv7s arm64"
	)
endfunction()


function(install_ios_target SDK_NAME)
	install(TARGETS ${SDK_NAME}
		FRAMEWORK DESTINATION Frameworks)
endfunction()
	