from pympidata import dataset
from pympidata import parser
import sys
import json
from tabulate import tabulate

def print_avgs(metrics):
    print 'AVG GLOBAL STATS\n'

    print tabulate([[metrics['avg']['app_time'],
                     metrics['avg']['mpi_time'],
                     float(metrics['avg']['mpi_time'])/metrics['avg']['app_time']*100]],headers=['App Time(ms)','MPI Time(ms)','MPI Time (%)'],tablefmt='orgtbl') 

    print '\nAVG RESULTS PER MPI OP\n'

    table = []
    for call in metrics['avg']['calls']:
        table.append([call,int(metrics['avg']['calls'][call]['#calls']),
                      int(metrics['avg']['calls'][call]['acc_time']),
                      int(metrics['avg']['calls'][call]['kbytes']),
                      int(metrics['avg']['calls'][call]['time_per_call']),
                      int(metrics['avg']['calls'][call]['bytes_per_call']) ])

    print tabulate(table,headers=['MPI call', '#Calls','Acc. Time(us)','Acc. Bytes','Time per call (us)','Bytes per call'],tablefmt='orgtbl')

    print '\nAVG RESULTS PER MPI CALL SITE\n'
    table = []
    for call in metrics['avg']['callsites']:
        table.append([call,int(metrics['avg']['callsites'][call]['#calls']),
                      int(metrics['avg']['callsites'][call]['acc_time']),
                      int(metrics['avg']['callsites'][call]['kbytes']),
                      int(metrics['avg']['callsites'][call]['time_per_call']),
                      int(metrics['avg']['callsites'][call]['bytes_per_call']) ])

    print tabulate(table,headers=['MPI callsite', '#Calls','Acc. Time(us)','Acc. Bytes','Time per call (us)','Bytes per call'],tablefmt='orgtbl')

dr = parser.DataReader() 
ds = dataset.Dataset(dr.read_files(sys.argv[1]))
ds.averages()
print_avgs(ds.metrics())
