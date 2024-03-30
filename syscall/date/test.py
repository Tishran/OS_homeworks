#!/usr/bin/env python3

import datetime as dt

from gradelib import *


runner = Runner(save('xv6.out'))


@test(1, 'test date')
def test_date():
    runner.run_qemu(shell_script([
        'date',
    ]))

    try:
        now = dt.datetime.now(dt.timezone.utc)

        output = runner.qemu.output.splitlines()
        date_result = output[output.index('$ date') + 1]
        date = dt.datetime.strptime(date_result + '+00:00', '%Y-%m-%dT%H:%M:%S%z')

        delta = dt.timedelta(seconds=5)
        if date - delta < now < date + delta:
            return
    except:
        pass

    raise Exception("Test failed")


run_tests()
