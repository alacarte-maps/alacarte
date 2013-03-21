#!/usr/bin/python
# -*- coding: utf-8 -*-

# README
# Diese Datei in den build-Ordner kopieren und dort ausführen.

import os

#print('\nDeleting old data')
#print('lcov -z -d ./CMakeFiles')
#os.system('lcov -z -d ./CMakeFiles')

os.system('./unitTests_eval')
os.system('./unitTests_general')
os.system('./unitTests_server')
os.system('./unitTests_importer')
os.system('./unitTests_utils')
os.system('./unitTests_mapcss')
os.system('./unitTests_parser')

try:
    os.mkdir('tracefiles')
except (OSError):
    pass
try:
    os.mkdir('html')
except (OSError):
    pass

for root, dirs, files in os.walk('CMakeFiles'):
    #print(files)
    for file in files:
        if file.endswith('.gcda') and file != 'geodata.cpp.gcda':
            print('\nProcessing ' + root + file + '\n')
            #os.system('gcov ' + root + file + ' -o coverage.info')
            print('lcov -c -q -d ' + root + '/' + file + ' -o  tracefiles/' + root.replace('/', '_') + '_' + file + '.info')
            os.system('lcov -c -q -d ' + root + '/' + file + ' -o tracefiles/' + root.replace('/', '_') + '_' + file + '.info')

print('\nMerging tracefiles\n')
files = os.listdir('tracefiles')
param = ' -a tracefiles/'.join(files)
print('lcov -a tracefiles/' + param + ' -o coverage.info')
os.system('lcov -a tracefiles/' + param + ' -o coverage.info')

print('\nGenerating HTML\n')
os.system('lcov --remove coverage.info "/usr*" "extras/*" "tests/*" -o coverage.info')
os.system('genhtml -o html coverage.info')
