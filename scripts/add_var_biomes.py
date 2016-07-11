import fileinput
import re
import sys
import os

from os import listdir
from os.path import isfile, join

if len(sys.argv) != 2:
  print "Use : python add_script_line_to_biomes.py <DirName/>"
  exit();

mypath = sys.argv[1]
var_name = "m_grassColor"
type_name = "sf::Color"
func_name = "getGrassColor()"
return_name = "sf::Color"
init_value = "m_tileStartColor"

def add_line_hpp(filename):
  for line in fileinput.input(mypath + filename, inplace=1):
    if line.find('getGrassSizeY()') != -1:
    #add prototype of the fonction
      print line,
      print "\tvirtual " + return_name + "\t\t\t\t\t\t\t\t\t" + func_name + ";"
    elif line.find('m_grassSizeY') != -1:
    #add variable definition
      print line,
      print "\t" + type_name + "\t\t\t\t\t\t\t\t\t\t\t" + var_name + ";"
    else:
      print line,

def add_line_cpp(filename):
  skip_line = False
  for line in fileinput.input(mypath + filename, inplace=1):
    if line.find('m_grassSizeY(') != -1:
    #init variable in constructor
      print line,
      print "\t" + var_name + "(" + init_value + "),"
    elif line.find('m_grassSizeY)') != -1:
      skip_line = True
      print line,
    elif skip_line == True:
    #add definition of the function
      print line,
      print "\n" + return_name + "\t" + os.path.splitext(filename)[0] + "::" + func_name + "\n",
      print "{\n",
      print "\treturn randomColor(" + var_name + ");\n",
      print "}\n",
      skip_line = False
    else:
      print line,

onlyfiles = [f for f in listdir(mypath) if isfile(join(mypath, f))]
for filename in onlyfiles:
  if (filename.startswith('ABiome') == False):
    if filename.find('Biome.hpp') != -1:
      add_line_hpp(filename)
    elif filename.find('Biome.cpp') != -1:
      add_line_cpp(filename)

