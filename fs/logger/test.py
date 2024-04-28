#!/usr/bin/env python3

import os
import signal
import subprocess
import time


class Logger:
    def __init__(self, path, prefix):
        self._prefix = prefix
        self._logger = subprocess.Popen([path, prefix], stdin=subprocess.PIPE, encoding='utf8')
        self._track = ['']
        time.sleep(1)

    def feed(self, string):
        self._logger.stdin.write(string)
        self._track[-1] += string

    def rotate(self):
        self._logger.stdin.flush()
        time.sleep(1)
        self._logger.send_signal(signal.SIGHUP)
        time.sleep(1)
        self._track.append('')

    def close(self):
        self._logger.stdin.close()
        self._logger.wait()
        assert self._logger.returncode == 0

    def verify(self, check_last=True):
        if not check_last:
            self._logger.stdin.flush()
        time.sleep(1)
        def content_differ(name, content):
            with open(name, 'r') as f:
                s = f.read()
                return s != content

        if check_last:
            filename = self._prefix
            content = self._track[-1]
            if content_differ(filename, content):
                self.close()
                raise AssertionError(f'Content in {filename} != {content}')

        for i in range(len(self._track) - 1):
            filename = f'{self._prefix}.{i + 1}'
            content = self._track[-2 - i]
            if content_differ(filename, content):
                self.close()
                raise AssertionError(f'Content in {filename} != {content}')


def test_logger():
    logger = Logger('./main', 'testlogs')
    logger.feed('Linux is a clone of the operating system Unix, written from scratch by\n')
    logger.feed('Linus Torvalds with assistance from a loosely-knit team of hackers across the Net.\n')
    logger.feed('It aims towards POSIX and Single UNIX Specification compliance.\n')
    logger.rotate()
    logger.feed('It has all the features you would expect in a modern fully-fledged Unix, including true multitasking, virtual memory, shared libraries, demand\n')
    logger.verify(False)
    logger.rotate()
    logger.feed('loading, shared copy-on-write executables, proper memory management, and multistack networking including IPv4 and IPv6.\n')
    logger.verify(False)
    logger.feed('It is distributed under the GNU General Public License v2 - see the')
    logger.feed('accompanying COPYING file for more details.')
    logger.feed('Hello world!!')
    logger.close()
    logger.verify()


if __name__ == '__main__':
    test_logger()
