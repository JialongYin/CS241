#!/usr/bin/python3
from sys import argv
from os.path import isfile
processes = []
counter = 0

LOGFILE = "../gthread.log"
def fail():
    print("Failed!")
    exit(1)
def main():
    if len(argv) != 2:
        print("Usage: python3 check_scheduler.py <scheme>")
        print("Acceptable schemes are: fcfs, sjf, psrtf, pri, ppri, rr")
        exit(1)

    lines = open(LOGFILE, 'r').readlines()
    lines = [l.split() for l in lines]
    if argv[1] == 'sjf':
        check_sjf(lines)
    elif argv[1] == 'psrtf':
        check_psrtf(lines)
    elif argv[1] == 'fcfs':
        check_fcfs(lines)
    elif argv[1] == 'rr':
        check_rr(lines)
    elif argv[1] == 'pri':
        check_pri(lines)
    elif argv[1] == 'ppri':
        check_ppri(lines)

def my_find(processes, name):
    for i in range(len(processes)):
        if processes[i]['name'] == name:
            return i

    return -1

# For comparators that can't get interrupted
def validate_simple_processes(lines, key):
    i = 0
    processes = []
    last_started = None
    while i < len(lines):
        if lines[i][0] == "Registered":
            processes.append({'name': lines[i][1], 'arrival':i,'length':lines[i][2], 'priority':lines[i][3]})
            i += 1
        elif lines[i][0] == "Switched":
            tid = lines[i][2]
            if tid == '000':
                break
            p = min(processes, key=key)
            assert(p['name'] == tid)
            last_started = tid
            i += 1
        elif lines[i][0] == "Ended":
            assert(lines[i][1] == last_started)
            idx = my_find(processes, tid)
            if idx == -1:
                fail()
            processes.pop(idx)
            i += 1

    assert(len(processes) == 0)
    print("Success!")
    return
            


def check_sjf(lines):
    validate_simple_processes(lines, lambda obj: (obj['length'], obj['arrival'])) 

def check_psrtf(lines):
    print("PSRTF not yet supported")

def check_fcfs(lines):
    validate_simple_processes(lines, lambda obj: (obj['arrival']))

def check_rr(lines):
    print("RR not yet supported")

def check_pri(lines):
    validate_simple_processes(lines, lambda obj: (obj['priority']))

def check_ppri(lines):
    print("PPRI not yet supported")

if __name__ == "__main__":
    main()

