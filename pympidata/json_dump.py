from pympidata import dataset
from pympidata import parser
import sys
import json

dr = parser.DataReader() 
ds = dataset.Dataset(dr.read_files(sys.argv[1]))
ds.averages()

print json.dumps(ds.metrics(),indent=4)
