import sys
import argparse
import cv2
import numpy as np

#This is a test change
print("This is a test change")

#Parse arguments and load additional modules
parser = argparse.ArgumentParser(description = 'Luna SDK realsense depth demo')
group = parser.add_argument_group('Required arguments')
group.add_argument('--data', required = True, help = 'absolute path to Luna SDK data directory')
group.add_argument('--bindPath', required = True, help = 'absolute path to Luna SDK bindings directory')
group.add_argument('--rsbindPath', required = True, help = 'absolute path to realsense bindings directory')
args = parser.parse_args()
print("Arguments are: data: {} bindPath: {} rsbindPath: {}".format(args.data, args.bindPath, args.rsbindPath))

sys.path += (args.bindPath, args.rsbindPath)
import FaceEngine as fe
import pyrealsense2 as rs

if __name__ == "__main__":
	#create Face Engine root object
	root = fe.createFaceEngine(args.data)
	#check license
	license = root.getLicense()
	if not root.activateLicense(license, args.data + "/license.conf"):
		print("Failed to activate license!")
		exit(-1)
	
	detector = root.createDetector(fe.FACE_DET_V1)
	estimator = root.createDepthEstimator()
	warper = root.createWarper()
	
	# Configure streams and start the pipeline
	pipe = rs.pipeline()
	pipeProfile = pipe.start()

	device = pipeProfile.get_device()
	print(device)

	# Since realsense depth sensors contain pixel values in hardware specific units
	# we have to manually convert it to milimeters for depth estimator to work properly
	depthSensor = device.first_depth_sensor()
	depthScale = depthSensor.get_depth_scale() * 1000
	print("Depth scale is {}".format(depthScale))

	#For warper to work properly, we need to align both stream viewports.
	align_to = rs.stream.color
	depthToColorAlignment = rs.align(align_to)

	#create window
	cv2.namedWindow('Realsense depth example', cv2.WINDOW_AUTOSIZE)
	
	# main processing loop
	try:
		while True:
			# Get frameset of color and depth
			frames = pipe.wait_for_frames()
	
			# Align the depth frame to color frame
			alignedFrames = depthToColorAlignment.process(frames)
	
			# Get aligned frames
			depthFrame = alignedFrames.get_depth_frame()
			rgbFrame = alignedFrames.get_color_frame()

			# Validate that both frames are valid
			if not depthFrame or not rgbFrame:
				continue
		
			#convert depth map to milimeters
			depthFrame = np.asanyarray(depthFrame.get_data())
			depthFrame = (depthFrame * depthScale).astype(np.int16)
			
			rgbFrame = np.asanyarray(rgbFrame.get_data())
			
			#convert incoming frames to SDK images
			depthImage = fe.Image()
			depthImage.setData(depthFrame, fe.FormatType.R16)

			rgbImage = fe.Image()
			rgbImage.setData(rgbFrame, fe.FormatType.R8G8B8)

			#perform detection
			err, face = detector.detectOne(rgbImage, rgbImage.getRect(), fe.DetectionType(fe.dt5Landmarks))

			#prepare cv image for visualisation
			cvRGBImage = cv2.cvtColor(rgbFrame, cv2.COLOR_BGR2RGB)

			if(err.isError):
				print("Failed to detect!")
			else:
				if(face.isValid() and face.landmarks5_opt.isValid()):	
					#warp depth image
					transformation = warper.createTransformation(face.detection, face.landmarks5_opt.value())
					warpResult, warpImage = warper.warp(depthImage, transformation)
					if warpResult.isError:
						print("Failed to warp image!")
					
					#perform depth liveness estimation
					error, output = estimator.estimate(warpImage)
					if(error.isOk):
						color = (0, 255, 0) if output.isReal else (0, 0, 255)
						box = face.detection.rect
						#draw bbox
						cv2.rectangle(cvRGBImage,
							(int(box.x), int(box.y)),
							(int(box.x + box.width), int(box.y + box.height)),
							color, 2)
						print(output)

			cv2.imshow('Realsense depth example', cvRGBImage)
			key = cv2.waitKey(1)
			# Press esc or 'q' to close the image window
			if key & 0xFF == ord('q') or key == 27:
				cv2.destroyAllWindows()
				break
	finally:
		pipe.stop()