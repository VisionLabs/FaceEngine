import os
import sys


def set_shared_lib_dir():

    if "FSDK_ROOT" in os.environ:
        path_to_fsdk_root = os.getenv('FSDK_ROOT')

        if os.name == 'nt' and sys.version_info >= (3, 8, 0):

            # find dlls for windows and python version >= 3.8.0
            os.add_dll_directory(os.path.join(path_to_fsdk_root, 'bin', 'vs2015', 'x64'))

        elif os.name == 'nt' and sys.version_info < (3, 8, 0):

            if "PATH" in os.environ and path_to_fsdk_root not in os.environ["PATH"]:
                os.environ["PATH"] += os.pathsep + path_to_fsdk_root
            else:
                os.environ["PATH"] = path_to_fsdk_root

        elif os.name == 'posix':

            if "LD_LIBRARY_PATH" in os.environ and path_to_fsdk_root not in os.environ["LD_LIBRARY_PATH"]:
                os.environ["LD_LIBRARY_PATH"] += os.pathsep + path_to_fsdk_root
            else:
                os.environ["LD_LIBRARY_PATH"] = path_to_fsdk_root

        else:
            print("Unknown platfrorm. Cannot set environment variable for shared libraries.")
