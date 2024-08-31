#!/usr/bin/env python3

import logging
import pandas
import numpy
from argparse import ArgumentParser

def get_regions_lifespan_by_color(region_dataframe):
	'''Return the lifespan of the regions by the TRACKED_COLOR'''
	
	# Using the value of the TRACKED_COLOR to identify regions through time
	# Compute the lifespan of the regions by taking their first date of observation and their last date of observation
	regions = region_dataframe.groupby('TRACKED_COLOR').aggregate(
		FIRST_OBSERVATION = pandas.NamedAgg(column='FIRST_DATE_OBS', aggfunc = 'min'),
		LAST_OBSERVATION = pandas.NamedAgg(column='DATE_OBS', aggfunc = 'max'),
	)
	
	regions['LIFESPAN'] = regions['LAST_OBSERVATION'] - regions['FIRST_OBSERVATION']
	
	return regions


# Start point of the script
if __name__ == '__main__':
	
	# Get the arguments
	parser = ArgumentParser(description='Compute the lifespan of regions from a CSV file containg the regions infos and write them to a new csv file')
	parser.add_argument('--output', '-o', default = 'regions_lifespan.csv', help = 'Path of the output CSV file (default to regions_lifespan.csv)')
	parser.add_argument('--update', '-u', action='store_true', help = 'Update the input file with the lifespan')
	parser.add_argument('--verbose', '-v', choices = ['DEBUG', 'INFO', 'ERROR'], default = 'INFO', help = 'Set the logging level (default is INFO)')
	parser.add_argument('filepath', metavar = 'FILEPATH', help = 'The path to the CSV file containg the regions as generated by the script aggregate_tables_from_fits.py')
	args = parser.parse_args()
	
	# Setup the logging
	logging.basicConfig(level = getattr(logging, args.verbose), format = '%(asctime)s %(levelname)-8s: %(message)s')
	
	# Create a dataframe from the region csv
	try:
		region_dataframe = pandas.read_csv(args.filepath, index_col = ['Filepath', 'Row'], parse_dates = ['FIRST_DATE_OBS', 'DATE_OBS'])
	except Exception as why:
		logging.critical('Could not read CSV file %s: %s', args.region_csv, why)
		raise
	
	regions_lifespan = get_regions_lifespan_by_color(region_dataframe)
	
	try:
		regions_lifespan.to_csv(args.output)
	except Exception as why:
		logging.critical('Could not write CSV file %s: %s', args.output, why)
		raise
	else:
		logging.info('Wrote CSV file %s', args.output)
	
	# Add the LIFESPAN column to the regions
	if args.update:
		region_dataframe['LIFESPAN'] = numpy.nan
		for tracked_color, lifespan in regions_lifespan.items():
			region_dataframe.loc[region_dataframe['TRACKED_COLOR']  == tracked_color, 'LIFESPAN'] = lifespan
		
		try:
			region_dataframe.to_csv(args.filepath)
		except Exception as why:
			logging.critical('Could not update CSV file %s: %s', args.filepath, why)
			raise
		else:
			logging.info('Updated CSV file %s', args.filepath)