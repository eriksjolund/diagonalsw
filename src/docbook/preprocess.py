import commands
import sys
import os
from xml.dom import minidom
path=os.getenv('PATH')

if len(sys.argv) != 4:
  sys.exit()

diagonalsw_dir=sys.argv[1]
inputfilename=sys.argv[2]
outputfilename=sys.argv[3]

os.putenv('PATH',"%s:%s " % (diagonalsw_dir,path)) 
#os.chdir('example_files')
xmldoc = minidom.parse(inputfilename)
reflist = xmldoc.getElementsByTagName('repl')

for i in reflist:
  shellcmd = i.getAttribute('shell-cmd')
  str=commands.getoutput( shellcmd )
  text = xmldoc.createTextNode( "[user@linux ~]$ %s\n%s" % ( shellcmd ,str) )
  parent = i.parentNode
  if parent == None:
    print "error\n"
  parent.replaceChild(text, i)


reflist = xmldoc.getElementsByTagName('repl-example-file')


xmlStr = xmldoc.toxml('utf-8')
file = open(outputfilename,"w")
file.writelines(xmlStr)



