#!/usr/bin/env python2.6
# -*- coding: iso-8859-15 -*-
import sys
import subprocess, shlex
import os
import os.path
import string
import logging
import argparse


# parameters for mencoder
ffmpeg_bin = '/pool/software/ffmpeg/bin/ffmpeg'

def run_ffmpeg(ffmpeg_cmd, input_filenames = []):
	
	logging.debug("About to execute: " + ' '.join(ffmpeg_cmd))
	try:
		# We start ffmpeg
		process = subprocess.Popen(ffmpeg_cmd, shell=False, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		
		# If we have input files, we write their content to the input pipe of ffmpeg
		for input_filename in input_filenames:
			try:
				with open(input_filename, 'rb') as input_file:
					logging.debug("Adding image %s", input_filename)
					process.stdin.write(input_file.read())
			except Exception, why:
				logging.error("Could not add image %s to video: %s", input_filename, str(why))
		
		# We terminate ffmpeg
		stdout, stderr = process.communicate()
		return_code = process.poll()
		if return_code != 0:
			raise Exception("Return code : {}\t StdOut: {}\t StdErr: {}".format(return_code, stderr, stdout))
	except Exception, why:
		logging.critical('Failed running command %s : %s', ' '.join(convert), str(why))
		return False
	else:
		return True


def png_to_mp4_video(input_filenames, output_filename, frame_rate = 24, video_title = None, video_size = None, video_bitrate = None):
	
	# We verify the extension
	output_path, extension = os.path.splitext(output_filename)
	if extension != ".mp4":
		output_filename = output_path + ".mp4"
		logging.warning("Output filename for mp4 video does not have extension .mp4, changing output filename to %s", output_filename)
	
	# We set up ffmpeg for the creation of mp4
	ffmpeg = [ffmpeg_bin, '-y', '-r', str(frame_rate), '-f', 'image2pipe', '-vcodec', 'png', '-i', '-', '-an', '-vcodec', 'libx264', '-preset', 'slow']
	if video_bitrate:
		ffmpeg.extend(['-maxrate', str(video_bitrate) + 'k'])
	
	if video_title:
		ffmpeg.extend(['-metadata', 'title=' + str(video_title)])
	
	if video_size:
		ffmpeg.extend(['-s', video_size])
	
	ffmpeg.append(output_filename)
	
	logging.info("Making video %s", output_filename)
	return run_ffmpeg(ffmpeg, input_filenames)


def png_to_webm_video(input_filenames, output_filename, frame_rate = 24, video_title = None, video_size = None, video_bitrate = None):
	
	# We verify the extension
	output_path, extension = os.path.splitext(output_filename)
	if extension != ".webm":
		output_filename = output_path + ".webm"
		logging.warning("Output filename for webm video does not have extension .webm, changing output filename to %s", output_filename)
	
	# We set up ffmpeg for the creation of webm
	ffmpeg = [ffmpeg_bin, '-y', '-r', str(frame_rate), '-f', 'image2pipe', '-vcodec', 'png', '-i', '-', '-an', '-vcodec', 'libvpx', '-quality', 'good', '-cpu-used', '0', '-qmin', '10', '-qmax', '42', '-threads', '2']
	if video_bitrate:
		ffmpeg.extend(['-maxrate', str(video_bitrate) + 'k', '-bufsize', str(2*video_bitrate) + 'k'])
	else:
		ffmpeg.extend(['-bufsize', '1000k'])
	
	if video_title:
		ffmpeg.extend(['-metadata', 'title=' + str(video_title)])
	
	ffmpeg.append(output_filename)
	
	logging.info("Making video %s", output_filename)
	return run_ffmpeg(ffmpeg, input_filenames)


def png_to_ogv_video(input_filenames, output_filename, frame_rate = 24, video_title = None, video_size = None, video_bitrate = None):
	
	# We verify the extension
	output_path, extension = os.path.splitext(output_filename)
	if extension != ".ogv":
		output_filename = output_path + ".ogv"
		logging.warning("Output filename for ogv video does not have extension .ogv, changing output filename to %s", output_filename)
	
	# We set up ffmpeg for the creation of ogv
	ffmpeg = [ffmpeg_bin, '-y', '-r', str(frame_rate), '-f', 'image2pipe', '-vcodec', 'png', '-i', '-', '-an', '-vcodec', 'libtheora', '-q:v', '7']
	if video_bitrate:
		ffmpeg.extend(['-maxrate', str(video_bitrate) + 'k'])
	
	if video_title:
		ffmpeg.extend(['-metadata', 'title=' + str(video_title)])
	
	ffmpeg.append(output_filename)
	
	logging.info("Making video %s", output_filename)
	return run_ffmpeg(ffmpeg, input_filenames)


def png_to_ts_video(input_filenames, output_filename, frame_rate = 24, video_title = None, video_size = None, video_bitrate = None, video_preset='slow'):
	
	# We verify the extension
	output_path, extension = os.path.splitext(output_filename)
	if extension != ".ts":
		output_filename = output_path + ".ts"
		logging.warning("Output filename for mp4 video does not have extension .mp4, changing output filename to %s", output_filename)
	
	# We set up ffmpeg for the creation of mp4
	ffmpeg = [ffmpeg_bin, '-y', '-r', str(frame_rate), '-f', 'image2pipe', '-vcodec', 'png', '-i', '-', '-an', '-vcodec', 'libx264', '-preset', video_preset, '-qp', '0']
	if video_bitrate:
		ffmpeg.extend(['-maxrate', str(video_bitrate) + 'k'])
	
	if video_title:
		ffmpeg.extend(['-metadata', 'title=' + str(video_title)])
	
	if video_size:
		ffmpeg.extend(['-s', video_size])
	
	ffmpeg.append(output_filename)
	
	logging.info("Making video %s", output_filename)
	return run_ffmpeg(ffmpeg, input_filenames)


def video_to_mp4_video(input_filenames, output_filename, frame_rate = 24, video_title = None, video_size = None, video_bitrate = None):
	
	# We verify the extension
	output_path, extension = os.path.splitext(output_filename)
	if extension != ".mp4":
		output_filename = output_path + ".mp4"
		logging.warning("Output filename for mp4 video does not have extension .mp4, changing output filename to %s", output_filename)
	
	# We set up ffmpeg for the creation of mp4
	ffmpeg = [ffmpeg_bin, '-y', '-i']
	if isinstance(input_filenames, basestring):
		 ffmpeg.append(input_filenames)
	elif len(input_filenames) == 1:
		 ffmpeg.append(input_filenames[0])
	else:
		ffmpeg.append("concat:"+'|'.join(input_filenames))
	
	ffmpeg.extend(['-an', '-vcodec', 'libx264', '-preset', 'slow', '-r', str(frame_rate)])
	
	if video_bitrate:
		ffmpeg.extend(['-maxrate', str(video_bitrate) + 'k'])
	
	if video_title:
		ffmpeg.extend(['-metadata', 'title=' + str(video_title)])
	
	if video_size:
		ffmpeg.extend(['-s', video_size])
	
	ffmpeg.append(output_filename)
	
	logging.info("Making video %s", output_filename)
	return run_ffmpeg(ffmpeg)

def video_to_webm_video(input_filenames, output_filename, frame_rate = 24, video_title = None, video_size = None, video_bitrate = None):
	
	# We verify the extension
	output_path, extension = os.path.splitext(output_filename)
	if extension != ".webm":
		output_filename = output_path + ".webm"
		logging.warning("Output filename for webm video does not have extension .webm, changing output filename to %s", output_filename)
	
	# We set up ffmpeg for the creation of webm
	ffmpeg = [ffmpeg_bin, '-y', '-i']
	if isinstance(input_filenames, basestring):
		 ffmpeg.append(input_filenames)
	elif len(input_filenames) == 1:
		 ffmpeg.append(input_filenames[0])
	else:
		ffmpeg.append("concat:"+'|'.join(input_filenames))
	
	ffmpeg.extend(['-an', '-vcodec', 'libvpx', '-quality', 'good', '-cpu-used', '0', '-qmin', '10', '-qmax', '42', '-threads', '2', '-r', str(frame_rate)])
	
	if video_bitrate:
		ffmpeg.extend(['-maxrate', str(video_bitrate) + 'k'])
	
	if video_title:
		ffmpeg.extend(['-metadata', 'title=' + str(video_title)])
	
	if video_size:
		ffmpeg.extend(['-s', video_size])
	
	ffmpeg.append(output_filename)
	
	logging.info("Making video %s", output_filename)
	return run_ffmpeg(ffmpeg)

def video_to_ogv_video(input_filenames, output_filename, frame_rate = 24, video_title = None, video_size = None, video_bitrate = None):
	
	# We verify the extension
	output_path, extension = os.path.splitext(output_filename)
	if extension != ".ogv":
		output_filename = output_path + ".ogv"
		logging.warning("Output filename for ogv video does not have extension .ogv, changing output filename to %s", output_filename)
	
	# We set up ffmpeg for the creation of ogv
	ffmpeg = [ffmpeg_bin, '-y', '-i']
	if isinstance(input_filenames, basestring):
		 ffmpeg.append(input_filenames)
	elif len(input_filenames) == 1:
		 ffmpeg.append(input_filenames[0])
	else:
		ffmpeg.append("concat:"+'|'.join(input_filenames))
	
	ffmpeg.extend(['-an', '-vcodec', 'libtheora', '-q:v', '7', '-r', str(frame_rate)])
	
	if video_bitrate:
		ffmpeg.extend(['-maxrate', str(video_bitrate) + 'k'])
	
	if video_title:
		ffmpeg.extend(['-metadata', 'title=' + str(video_title)])
	
	if video_size:
		ffmpeg.extend(['-s', video_size])
	
	ffmpeg.append(output_filename)
	
	logging.info("Making video %s", output_filename)
	return run_ffmpeg(ffmpeg)


def setup_logging(filename = None, quiet = False, verbose = False, debug = False):
	global logging
	if debug:
		logging.basicConfig(level = logging.DEBUG, format='%(levelname)-8s: %(message)s')
	elif verbose:
		logging.basicConfig(level = logging.INFO, format='%(levelname)-8s: %(message)s')
	else:
		logging.basicConfig(level = logging.CRITICAL, format='%(levelname)-8s: %(message)s')
	
	if quiet:
		logging.root.handlers[0].setLevel(logging.CRITICAL + 10)
	elif verbose:
		logging.root.handlers[0].setLevel(logging.INFO)
	else:
		logging.root.handlers[0].setLevel(logging.CRITICAL)
	
	if filename:
		fh = logging.FileHandler(filename, delay=True)
		fh.setFormatter(logging.Formatter('%(asctime)s %(name)-12s %(levelname)-8s %(funcName)-12s %(message)s', datefmt='%Y-%m-%d %H:%M:%S'))
		if debug:
			fh.setLevel(logging.DEBUG)
		else:
			fh.setLevel(logging.INFO)
		
		logging.root.addHandler(fh)


# Start point of the script
if __name__ == "__main__":
	
	script_name = os.path.splitext(os.path.basename(sys.argv[0]))[0]
	
	known_video_types = ['.ts', '.mp4', '.ogv', '.webm']
	
	# Get the arguments
	parser = argparse.ArgumentParser(description='Make video from png.')
	parser.add_argument('--debug', '-d', default=False, action='store_true', help='Set the logging level to debug')
	parser.add_argument('--quiet', '-q', default=False, action='store_true', help='Do not display any error message.')
	parser.add_argument('--overwrite', '-o', default=False, action='store_true', help='Overwrite the video if it already exists')
	parser.add_argument('--frame_rate', '-r', default=24, type=int, help='Frame rate for the video')
	parser.add_argument('--video_bitrate', '-b', default=0, type=int, help='A maximal bitrate for the video')
	parser.add_argument('--video_title', '-t', default=None, help='A title for the video')
	parser.add_argument('--video_size', '-s', default=None, help='The size of the video. Must be specified like widthxheight in pixels')
	parser.add_argument('--video_filenames', '-f', action='append', help='The filenames for the video. Must end in ' + ', '.join(known_video_types))	
	parser.add_argument('sources', nargs='+', help='The paths of the source png images/video(s).')
	
	args = parser.parse_args()
	
	# Setup the logging
	setup_logging(quiet = args.quiet, verbose = True, debug = args.debug)
	
	if not args.video_filenames:
		logging.error("You must specify at least one video filename")
		sys.exit(2)
	
	videos_filenames = dict()
	for video_filename in args.video_filenames:
		video_path, video_extension = os.path.splitext(video_filename)
		if video_extension in known_video_types:
			if video_extension in videos_filenames:
				logging.warning("Video of type %s has been specfied more than once as argument, using last one %s", video_extension, video_filename)
			videos_filenames[video_extension] = video_filename
		else:
			logging.error("Unknown video type %s for video %s", video_extension, video_filename)
	
	for video_filename in videos_filenames.values():
		if video_filename and os.path.exists(video_filename):
			if not args.overwrite:
				logging.critical("Video %s already exists, not overwriting", video_filename)
				sys.exit(1)
			else:
				logging.info("Video %s will be overwritten", video_filename)
	
	source_path, source_extension = os.path.splitext(args.sources[0])
	
	video_sources = list()
	
	if source_extension == '.png':
		logging.info("Source type detected is png images")
		# We make a video from png images
		if videos_filenames['.ts']:
			png_to_ts_video(args.sources, videos_filenames['.ts'], frame_rate = args.frame_rate, video_title = args.video_title, video_size = args.video_size, video_bitrate = args.video_bitrate)
			video_sources.append(videos_filenames['.ts'])
		
		elif videos_filenames['.mp4']:
			png_to_mp4_video(args.sources, videos_filenames['.mp4'], frame_rate = args.frame_rate, video_title = args.video_title, video_size = args.video_size, video_bitrate = args.video_bitrate)
			video_sources.append(videos_filenames['.mp4'])
		
		elif videos_filenames['.webm']:
			png_to_webm_video(args.sources, videos_filenames['.webm'], frame_rate = args.frame_rate, video_title = args.video_title, video_size = args.video_size, video_bitrate = args.video_bitrate)
			video_sources.append(videos_filenames['.webm'])
		
		elif videos_filenames['.ogv']:
			png_to_ogv_video(args.sources, videos_filenames['.ogv'], frame_rate = args.frame_rate, video_title = args.video_title, video_size = args.video_size, video_bitrate = args.video_bitrate)
			video_sources.append(videos_filenames['.ogv'])
	else:
		logging.info("Source type detected is videos")
		video_sources = args.sources
	
	# We make the remaining videos from video sources
	if videos_filenames['.mp4']:
		video_to_mp4_video(video_sources, videos_filenames['.mp4'], frame_rate = args.frame_rate, video_title = args.video_title, video_size = args.video_size, video_bitrate = args.video_bitrate)

	elif videos_filenames['.webm']:
		video_to_webm_video(video_sources, videos_filenames['.webm'], frame_rate = args.frame_rate, video_title = args.video_title, video_size = args.video_size, video_bitrate = args.video_bitrate)

	elif videos_filenames['.ogv']:
		video_to_ogv_video(video_sources, videos_filenames['.ogv'], frame_rate = args.frame_rate, video_title = args.video_title, video_size = args.video_size, video_bitrate = args.video_bitrate)

