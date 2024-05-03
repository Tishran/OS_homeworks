#!/usr/bin/env python3

import subprocess
import time


def run(cmd):
    start = time.time()
    subprocess.check_call(cmd)
    end = time.time()
    return end - start


def test_sse():
    nosse_elapsed = run(['./main'])
    print('Time without sse', nosse_elapsed)
    sse_elapsed = run(['./main', '--sse'])
    print('Time with sse', sse_elapsed)
    assert sse_elapsed < nosse_elapsed / 1.5


if __name__ == '__main__':
    test_sse()
