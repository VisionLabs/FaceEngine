
# parse_version Macro
# This macro converts a version string into its three integer components.
#
# Usage:
#     parse_version(MAJOR MINOR PATCH ${VERSION})
#
# Parameters:
#     MAJOR      The variable to store the major integer component in.
#     MINOR      The variable to store the minor integer component in.
#     PATCH      The variable to store the patch integer component in.
#     VERSION    The version string to convert ("v.#.#.#" format).
macro(parse_version MAJOR MINOR PATCH VERSION)
    string(REGEX REPLACE "v.([0-9]+)\\..*" "\\1" ${MAJOR} ${VERSION})
    string(REGEX REPLACE "v.[0-9]+\\.([0-9]+).*" "\\1" ${MINOR} ${VERSION})
    string(REGEX REPLACE "v.[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" ${PATCH} ${VERSION})
endmacro()

# get and parse_version Macro
# This macro gets current tag info from git and converts a version string 
# into its three integer components.
#
# Usage:
#     get_and_parse_version(MAJOR MINOR PATCH VERSION HASH)
#
# Parameters:
#     MAJOR      The variable to store the major integer component in.
#     MINOR      The variable to store the minor integer component in.
#     PATCH      The variable to store the patch integer component in.
#     HASH       The sha hash of the current head revision.

function(get_and_parse_version MAJOR MINOR PATCH BUGFIX HASH)
	# Get version info from the git tags
	include(GetGitRevisionDescription)
	get_git_head_revision(GIT_REFSPEC GIT_HASH)
	git_describe(VERSION --tags)

	string(REGEX REPLACE "v.([0-9]+)\\..*" "\\1" GIT_MAJOR ${VERSION})
	string(REGEX REPLACE "v.[0-9]+\\.([0-9]+).*" "\\1" GIT_MINOR ${VERSION})
	string(REGEX REPLACE "v.[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" GIT_PATCH ${VERSION})
	string(REGEX REPLACE "v.[0-9]+\\.[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" GIT_BUGFIX ${VERSION})

	set(${MAJOR}  "${GIT_MAJOR}"  PARENT_SCOPE)
	set(${MINOR}  "${GIT_MINOR}"  PARENT_SCOPE)
	set(${PATCH}  "${GIT_PATCH}"  PARENT_SCOPE)
	set(${BUGFIX} "${GIT_BUGFIX}" PARENT_SCOPE)
	
	set(${HASH}  "${GIT_HASH}"  PARENT_SCOPE)
endfunction()