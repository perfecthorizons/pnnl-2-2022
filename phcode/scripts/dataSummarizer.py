#!/usr/bin/env python3

import os
import sys
import csv
import json
from datetime import datetime

def usage(exit_code=0):
   '''Prints a usage message and exits with specified error code'''
   progname = os.path.basename(sys.argv[0])
   print('Usage: {} [LOGDIR] [SUMFILE]'.format(progname))
   sys.exit(exit_code)

def readXYData(filename):
   '''Reads X and Y data from log file'''
   if filename is None:
      return None

   print('\tReading XY data from {}'.format(filename))

   with open(filename, 'r') as inFile:
      reader = csv.reader(inFile)
      return list(reader)
   

def summarizeData(datafile, sumfile):
   '''Summarizes all data for a given log file'''

   if datafile is None or sumfile is None:
      return

   xyData = readXYData(datafile)
   if xyData is None:
      return
   
   if os.path.exists(sumfile):
      os.remove(sumfile)

   with open(sumfile, 'w+') as outFile:


      sumX = 0
      sumY = 0
      sumXabs = 0
      sumYabs = 0
      
      count = 0

      sumData = {}

      # Sum up data values
      for xy in xyData:
         if len(xy) != 2:
            continue

         try:
            x = float(xy[0])
            y = float(xy[1])
         except ValueError:
            print('Could not convert data values in data file to float')
            sys.exit(1)

         sumX += x
         sumY += y

         sumXabs += abs(x)
         sumYabs += abs(y)
         
         count += 1

      # Get averages
      sumData['ph_avgX'] = sumX / count
      sumData['ph_avgY'] = sumY / count
      sumData['ph_avgXabs'] = sumXabs / count
      sumData['ph_avgYabs'] = sumYabs / count


      json.dump(sumData, outFile)


   
def main():

   now = datetime.now()
   current_time = now.strftime('%H:%M:%S')

   print('Running data summarizer at {}'.format(current_time))

   # Parse command line args
   if len(sys.argv) != 3:
      usage(1)

   # Name of directory where logs are stored
   logdir = sys.argv[1]

   # Name of file to store summary in
   sumfile = sys.argv[2]

   # Get sorted list of log files in this directory
   try:
      logfiles = sorted(os.listdir(logdir))
   except:
      print('\tERROR: Unable to get list of files in dir: {}'.format(logdir))
      sys.exit(1)
   
   num = len(logfiles)
   print('\tFound {} files in {}'.format(str(num),logdir))

   # Remove any data files prior to the current day
   if num > 24:
      for lf in logfiles:
         print('\tRemoving old file: {}'.format(lf))
         os.remove(lf)
         num -= 1

         if num <= 24:
            break

   # Summarize most recently finished data file
   if num < 0:
      print('\tNo files found to summarize -- exiting...')
      sys.exit(0)
   elif num < 2:
      print('\tOnly one file found -- assuming it is still being written -- exiting...')
      sys.exit(0)


   summarizeData(logdir + logfiles[-2], sumfile)

   print('\tFinished summarizing data')

			

if __name__ == '__main__':
   main()

# vim: set sts=3 sw=3 ts=3 expandtab ft=python
