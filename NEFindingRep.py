#!/usr/bin/env python3

import re
import subprocess
import argparse
import os
hash_path = './HashClausesSuperMod/Hash_clauses'

def createParser():
	parser = argparse.ArgumentParser()
	parser.add_argument('cnf', type=str, help='Путь к кнф')
	parser.add_argument('-re', '--repeat', type=int, default=0, help='Сколько раз повторить')
	return parser

parser = createParser()
namespace = parser.parse_args()

def readingCnfFile(path_in, path_out):
	f = open(path_in)
	o = open(path_out, "w")
	line = f.readline()
	while line:
		res = findingCnf(line)
		if(res!=''):
			o.write(res)
		line=f.readline()
	f.close()
	o.close()

def findingCnf(str):
	pattern = re.compile('((-?\d+\s)+0\s)')
	res = ''
	for m in pattern.finditer(str):
		res += m.group()
		res += "\n"
	return res

def hashClausesSeed(path_in, path_out):
	command = hash_path + ' ' + path_in
	hash_output = open(path_out, 'w')
	process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,universal_newlines=True)
	for line in process.stdout.readlines():
		str_line = str(line)
		hash_output.write(str_line)
	retval = process.wait()
	hash_output.close
#=========================================
#temp1 = "findingRepResult"
#temp2 = "temp"
from random import choice
from string import digits

temp1 = (''.join(choice(digits) for i in range(12)))
temp2 = (''.join(choice(digits) for i in range(12)))
readingCnfFile(namespace.cnf, temp1)
command = hash_path + ' ' + temp1
process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,universal_newlines=True)
for line in process.stdout.readlines():
	if(findingCnf(line)!=''):
		print(line, end='')
#for i in range(0, namespace.repeat):
#	hashClausesSeed(temp1, temp2)
#	readingCnfFile(temp2, temp1)
os.remove(temp1)
