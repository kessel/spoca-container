#!/usr/bin/env python
import xml.dom.minidom
from datetime import datetime, timedelta
import dateutil.parser
import re
import sys
import os.path
import argparse
import matplotlib.dates as mdates
import numpy as np
import matplotlib.pyplot as plt
from get_events_stats import *
import logging
import threading, Queue, copy
from glob import glob


timedelta_regex = re.compile(r'((?P<days>\d+?)d)?((?P<hours>\d+?)h)?((?P<minutes>\d+?)m)?')
def parse_duration(duration):
	
	parts = timedelta_regex.match(duration)
	if not parts:
		log.critical("Cannot convert duration to timedelta : " + str(duration))
		return None
	
	parts = parts.groupdict()
	time_params = {}
	for (name, param) in parts.iteritems():
		if param:
			time_params[name] = int(param)
	return timedelta(**time_params)

# Return a time in the form yyyymmdd_hhmmss 
def pretty_date(date):
	return date.strftime("%Y%m%d_%H%M%S")


gradient = ["#f0f01e","#0000f0","#007800","#5af0f0","#f00096","#960000","#965af0","#009696","#5af03c","#f0785a","#5a0096","#969600","#f03c00","#00f000","#1e78f0","#f05ab4","#1ef096","#001e96","#7800f0","#00d2f0","#f000f0","#f09600","#f0f078","#b41e5a","#96f000","#1eb43c","#f0f000","#001ef0","#3c3cb4","#78f0f0","#b41eb4","#780000","#7878f0","#007878","#78f05a","#f0965a","#5ad21e","#787800","#f00000","#b43c00","#0078f0","#f078f0","#1ed296","#000078","#5a00f0","#00f0f0","#3cb4f0","#d29600","#f0f05a","#d2005a","#780078","#009600","#00f03c","#b4f000","#001ed2","#f05a96","#d200f0","#3c5ad2","#963cd2","#007896","#78f078","#f05a3c","#5ad200","#96001e","#96d23c","#f01e00","#965a00","#f05af0","#00d296","#1eb45a","#5a00d2","#5af0d2","#00b4f0","#f0b41e","#5a96f0","#f0005a","#960078","#1e9600","#00f01e","#d2f000","#000096","#f03c96","#f000d2","#003cf0","#963cf0","#1e78b4","#5af078","#d25a3c","#5af000","#b4001e","#b4f05a","#96b41e","#b47800","#d25ad2","#00f096","#00965a","#5a1eb4","#3cf0f0","#0096f0","#f0b43c","#5a78f0","#3c00f0","#b40078","#3c9600","#1ef03c","#00d200","#0000b4","#f07878","#d200d2","#f01e96","#961ef0","#1e5ab4","#5af096","#f03c1e","#78f000","#961e1e","#96f05a","#1eb4b4","#967800","#f05ad2","#b43cb4","#00963c","#b4d21e","#5ad2f0","#00f0d2","#f0d25a","#5a5af0","#5a00b4","#f0961e","#d2003c","#00f05a","#1ed200","#0000d2","#5ad23c","#d25a5a","#f000b4","#9600f0","#005a96","#005af0","#d23c00","#3cf096","#3c96f0","#d21e78","#00b496","#789600","#f03cf0","#d23cb4","#00961e","#b4f01e","#5ad2d2","#1ef0f0","#d2d25a","#785af0","#780096","#f0b400","#96003c","#3c3cd2","#1ef000","#1e0096","#78d23c","#d2783c","#1ed25a","#b400f0","#f0001e","#005ab4","#963c00","#f05a00","#1e96f0","#f00078","#00d2b4","#5a9600","#1e9696","#f03cd2","#f05a78","#00b41e","#5af0b4","#1e00f0","#d2f05a","#b45af0","#961e96","#f0d200","#b4b41e","#5a5ad2","#3cf000","#3c0096","#78f03c","#f0963c","#1ef05a","#b400d2","#d2001e","#003cb4","#b43c3c","#d25a00","#5ab4f0","#d20078","#00f0b4","#781ef0","#0096d2","#009678","#f03cb4","#1e961e","#3cd296","#961e00","#d2f03c","#d25af0","#1e1ef0","#78d200","#d2b400","#96961e","#f03c5a","#3c00b4","#3c78d2","#f0b45a","#3cf03c","#9600b4","#00d23c","#003c96","#d21e1e","#f0781e","#3cd2f0","#96005a","#b45a00","#783cf0","#00b4d2","#005ad2","#f01eb4","#1eb400","#1eb478","#1ef0b4","#b4f03c","#d23cf0","#00f078","#78d21e","#d2d200","#b4961e","#d23c78","#1e00d2","#3c78f0","#f0d23c","#5af05a","#7800b4","#3cf01e","#1e1e96","#d20000","#f01e3c","#f0783c","#b4005a","#b43c1e","#5a3cd2","#0096b4","#003cd2","#d200b4","#00b400","#00b45a","#3cf0d2","#00d2d2","#b43cf0","#f01ef0","#96f01e","#78b400","#b49600","#d23c96","#b4b43c","#3c96d2","#f0f03c","#3cf078","#961eb4","#3cd21e","#1e00b4","#b40000","#f0003c","#f05a5a","#f07800","#d23c1e","#5a3cf0","#1eb496","#0078b4","#1e5af0","#1e3cb4","#00d25a","#d20096","#1ef0d2","#00d21e","#d21ef0","#78f01e","#b4d200","#b4781e","#78b41e","#d2b43c","#1eb4f0","#b4003c","#5ad25a","#781eb4","#3cd200","#3c00d2","#3cd2b4","#f01e5a","#b41e00","#960096","#f05a1e","#f01e1e","#00b4b4","#5a1ef0","#1e78d2","#001eb4","#1ef078","#d21e96","#1e3cf0","#00b43c","#b41ef0","#96f03c","#f01ed2","#b4b400","#d2f01e","#d2963c","#1ed2f0","#b45a1e","#b41e3c","#00b478","#5ab41e","#1ef01e","#3cf0b4","#3c1eb4","#5af01e","#3cb4b4","#783cd2","#f03c3c","#d21e00","#7800d2","#0078d2","#3cd25a","#b400b4","#f01e78","#3c5af0","#1eb41e","#b43cd2","#3c1ef0","#b41e78","#96d200","#f0d21e","#d2961e","#b4d23c","#1ed2d2","#1e3cd2","#00d278","#5ab400","#d25a1e","#b41e1e","#d21e3c","#d21eb4","#1e96b4","#1e1eb4","#1ed23c","#5a1ed2","#9600d2","#d27800","#3cd278","#b40096","#f03c78","#3cf05a","#3cb41e","#b41ed2","#d23cd2","#3c3cf0","#96b400","#96d21e","#d2b41e","#d2d23c","#3cd2d2","#1e5ad2","#d23c5a","#1ed2b4","#1eb4d2","#d2781e","#1ed21e","#d2d21e","#b41e96","#1e1ed2","#3cd23c","#781ed2","#1e96d2","#d21e5a","#1ed278","#3cb400","#d21ed2","#3c1ed2","#3cb43c","#3cb4d2","#d23c3c","#961ed2"]
linestyles = ['-' , '--' , '-.' , ':']

def attributes_figure(meta_events, attributes, size=None, legend=False):
	""" Procedure to plot several attributes in a same time frame. The plot is saved to filename"""
	if size:
		figure = plt.figure(figsize=(size[0]/100., size[1]/100.), dpi=100)
	else:
		figure = plt.figure()
	
	if legend:
		figure.subplots_adjust(bottom=.1, hspace=.18, top=.95, left = .07, right = .85)
	else:
		figure.subplots_adjust(bottom=.1, hspace=.18, top=.95, left = .07, right = .95)
	
	# We precreate all the axes
	# Could be more easily done with plt.subplots but it is only available from version 1.0.1
	last_axes = None
	axes = dict()
	for p, attribute in enumerate(attributes):
		last_axes = axes[attribute] = figure.add_subplot(len(attributes), 1, p+1, title=str(attribute), sharex=last_axes)
	
	lines = list()
	labels = list()
	
	# We make the plots
	for attribute in attributes:
		
		ymin, ymax = float('inf'), float('-inf')
		
		for color, meta_event in meta_events.items():
			
			times, values = meta_event.get(('time', attribute))
			if(times):
				label = str(color)
				line = axes[attribute].plot(times, values, color=gradient[color % len(gradient)], label=label)
				
				if isinstance(line, (tuple, list)):
					# We ploted a tuple
					for i, l in enumerate(line):
						l.set_linestyle(linestyles[i])
					if label not in labels:
						lines.append(line[0])
						labels.append(label)
				
				elif label not in labels:
					lines.append(line)
					labels.append(label)
				
				values = [v for v in values if v != None]
				if values:
					if isinstance(values[0], (tuple, list)):
						for v in values:
							ymin = min(ymin, min(v))
							ymax = max(ymax, max(v))
					
					else:
						ymin = min(ymin, min(values))
						ymax = max(ymax, max(values))
		
		# We set the ylimits on the axes
		# We stretch a little the range
		stretch = (ymax - ymin) * 0.05
		
		if ymin >= 0 and ymin < stretch:
			ymin = 0
		else:
			ymin -= stretch
		axes[attribute].set_ylim((ymin, ymax+stretch), auto=False)
		
	
	# We set up the ticks labels on the last axes
	last_axes.xaxis.set_major_locator(mdates.HourLocator(byhour=[0]))
	last_axes.xaxis.set_major_formatter(mdates.DateFormatter('%b %d'))
	for tick in last_axes.xaxis.get_major_ticks():
		tick.label.set_rotation(45)
		tick.label.set_fontweight(700)
		tick.label.set_horizontalalignment('right')
	
	last_axes.xaxis.set_minor_locator(mdates.HourLocator(byhour=[6,12,18]))
	last_axes.xaxis.set_minor_formatter(mdates.DateFormatter('%H:%M'))
	for tick in last_axes.xaxis.get_minor_ticks():
		tick.label.set_rotation(45)
		tick.label.set_horizontalalignment('right')
	
	# We hide the tick labels on the previous axes
	for attribute in attributes[:-1]:
		plt.setp(axes[attribute].get_xmajorticklabels() + axes[attribute].get_xminorticklabels(), visible=False)
	
	if legend:
		figure.legend(lines, labels, loc='center right', fancybox=True, ncol=1)
	
	
	return figure


def save_figure(suffix, figure, time_ticks, max_delta, start_time, end_time): 
	
	# For each time tick we save a figure
	while not terminate_threads:
		
		time_tick = time_ticks.get()
		
		# We draw lines on the figure axes at the time tick
		for axes in figure.axes:
			axes.axvline(x=time_tick, linewidth=4, color='black', alpha=0.3)
	
		# We set up the time boundaries
		if time_tick - max_delta/2 < start_time:
			tmin = start_time
		elif time_tick + max_delta/2 > end_time:
			tmin = max(end_time - max_delta, start_time)
		else:
			tmin = time_tick - max_delta/2
	
		tmax = tmin + max_delta
		for axes in figure.axes:
			axes.set_xlim((tmin, tmax), auto=False)
	
		filename = pretty_date(time_tick) + '.' + suffix
		log.info("Saving partial figure for time " + str(time_tick) + " as " +  filename)
		figure.savefig(filename)
		
		time_ticks.task_done()
		
		# We erase the lines
		for axes in figure.axes:
			del axes.lines[-1]


# Start point of the script
if __name__ == "__main__":
	
	# Get the arguments
	parser = argparse.ArgumentParser(description='Generate a graph of the voevents.')
	parser.add_argument('--output', '-o', default='events.png', help='Name of the file for graph.')
	parser.add_argument('--attribute', '-a', metavar='key', default=[], action='append', help='The desired event attributes you wish to plot. Can be specified multiple times to have more than one plot in the graph.')
	parser.add_argument('--minlifetime', '-l', default=None, help='The minimal lifetime of the events to be plotted. Specify as 0d0h0m0s')
	parser.add_argument('--estimated_lifetime', '-e', default=False, action='store_true', help='Use the estimated lifetime of the events instead of the real lifetime.')
	parser.add_argument('--size', '-s', default=None, help='The size of the plot. Specify as widthxheight')
	parser.add_argument('--multi', '-M', nargs='?', default=None, const='', help='To have multiple graphs, one for each time. Specify a max duration like 0d0h0m0s')
	parser.add_argument('--legend', '-L', default=False, action='store_true', help='Set to add a legend')
	parser.add_argument('--threading', '-t', default=1, type=int, help='Set to a value bigger than 1 if you want multithreading.')
	parser.add_argument('--debug', '-d', default=False, action='store_true', help='Set the logging level to debug')
	parser.add_argument('--verbose', '-v', default=False, action='store_true', help='Set the logging level to verbose')
	parser.add_argument('filename', nargs='+', help='The paths of the files')
	args = parser.parse_args()
	
	# Create logger
	if args.debug:
		logging.basicConfig(level = logging.DEBUG, format='%(levelname)-8s: %(message)s')
	elif args.verbose:
		logging.basicConfig(level = logging.INFO, format='%(levelname)-8s: %(message)s')
	else:
		logging.basicConfig(level = logging.CRITICAL, format='%(levelname)-8s: %(message)s')
	
	log = logging.getLogger('plot_class_centers')
	
	
	
	# Parse the arguments
	attributes = args.attribute
	output = args.output
	
	filenames = list()
	for filename in args.filename:
		filenames.extend(glob(filename))
	
	minlifetime = None
	if args.minlifetime:
		minlifetime = parse_duration(args.minlifetime)
	
	if args.size:
		m = re.search(r'(\d+)\D(\d+)', args.size)
		if not m:
			log.critical("Error in size specification")
			sys.exit(2)
		size = (int(m.group(1)), int(m.group(2)))
	else:
		size=None
	
	# We parse the events from the filenames
	events = get_events(filenames)
	
	# We create the meta events
	meta_events = dict()
	for event in events:
		if event.color in meta_events:
			meta_events[event.color] += event
		else:
			meta_events[event.color] = MetaEvent(event.color, [event])
	
	# We remove the meta_events that do not have a sufficient lifetime
	if minlifetime and args.estimated_lifetime:
		for color in meta_events.keys():
			if meta_events[color].estimated_lifetime() < minlifetime:
				del meta_events[color]
	elif minlifetime:
		for color in meta_events.keys():
			if meta_events[color].lifetime() < minlifetime:
				del meta_events[color]
	
	# We write the colors of the meta_events in a file
	try :
		f = open('colors.txt', 'w+')
		f.write(str(meta_events.keys()))
		f.close()
	except IOError, why:
		log.critical("Could not write colors to file colors.txt")
	
	
	if args.attribute:
		# We create the big figure
		figure = attributes_figure(meta_events, args.attribute, legend=args.legend, size=size)
		
		# We save the full figure
		log.info("Saving full figure as " + str(args.output))
		figure.savefig(args.output)
		
		# If the user requested multi plots
		if args.multi != None:
			
			# We create the collection of all time ticks
			time_ticks = set()
			for meta_event in meta_events.values():
				times = meta_event.get('time')
				time_ticks.update(times)
			
			time_ticks = list(time_ticks)
			time_ticks.sort()
			
			max_delta = parse_duration(args.multi)
			
			if args.threading > 1:
				
				terminate_threads = False
				time_ticks_queue = Queue.Queue()
				threads = list()
				while len(threads) < args.threading:
					if figure == None:
						figure = attributes_figure(meta_events, args.attribute, legend=args.legend, size=size)
					threads.append(threading.Thread(group=None, target=save_figure, name='save_figure_'+str(len(threads)), args=(args.output, figure, time_ticks_queue, max_delta, time_ticks[0], time_ticks[-1]), kwargs={}))
					threads[-1].daemon = True
					threads[-1].start()
					figure = None
				
				for time_tick in time_ticks:
					time_ticks_queue.put(time_tick)
				try:
					time_ticks_queue.join()
				except Exception, why:
					terminate_threads = True
					log.critical("Stopping after error : " + str(why))
			else:
				# For each time tick we save a figure
				for time_tick in time_ticks:
				
					# We draw lines on the figure axes at the time tick
					for axes in figure.axes:
						axes.axvline(x=time_tick, linewidth=4, color='black', alpha=0.3)
			
					# We set up the time boundaries
					if time_tick - max_delta/2 < time_ticks[0]:
						tmin = time_ticks[0]
					elif time_tick + max_delta/2 > time_ticks[-1]:
						tmin = max(time_ticks[-1] - max_delta, time_ticks[0])
					else:
						tmin = time_tick - max_delta/2
			
					tmax = tmin + max_delta
					for axes in figure.axes:
						axes.set_xlim((tmin, tmax), auto=False)
			
					filename = pretty_date(time_tick)+'.'+args.output
					log.info("Saving partial figure for time " + str(time_tick) + " as " +  filename)
					figure.savefig(filename)
			
					# We erase the lines
					for axes in figure.axes:
						del axes.lines[-1]
