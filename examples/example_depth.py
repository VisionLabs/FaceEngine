import sys
import argparse
import cv2

#Parse arguments and load additional modules
parser = argparse.ArgumentParser(description = 'Luna SDK realsense depth demo')
group = parser.add_argument_group('Required arguments')
group.add_argument('--data', required = True, help = 'absolute path to Luna SDK data directory')
group.add_argument('--lunaPath', required = True, help = 'absolute path to Luna SDK bindings directory')
group.add_argument('--rsPath', required = True, help = 'absolute path to realsense bindings directory')
args = parser.parse_args()
print("Arguments are: data: {} lunaPath: {} rsPath: {}".format(args.data, args.lunaPath, args.rsPath))

sys.path += (args.lunaPath, args.rsPath)
import FaceEngine as fe
import pyrealsense2 as rs

if __name__ == "__main__":
	root = fe.createFaceEngine(args.data)
	#check license
	license = root.getLicense()
	if not root.activateLicense(license, args.data + "/license.conf"):
		print("Failed to activate license!")
		exit(-1)
	
	detector = root.createDetector(fe.FACE_DET_V1)
	warper = root.createWarper()
	

