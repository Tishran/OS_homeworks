#!/bin/bash

set -ex

export TMP=$(mktemp -d)

(cd env_test && ../main 1)

if [[ ! -f $TMP/done ]]; then
    echo "environment is broken"
    exit 1
fi

for i in 1 2 10; do
    export TMP=$(mktemp -d)

    (cd simple_test && ../main $i)

    if [[ ! -f $TMP/a ]] || [[ ! -f $TMP/b ]]; then
        echo "some of commands didn't run"
        exit 1
    fi
done

(cd pstree_test && ../main 10)

export TMP=$(mktemp -d)

(cd two_dirs_test && ../main 10)

if [[ $(uniq $TMP/out | wc -l) == 2 ]]; then
    echo "two recursive make commands should run in paraller"
    exit 1
fi

export TMP=$(mktemp -d)

(cd limit_test && ../main 2)

if [[ -f $TMP/limit ]]; then
    echo "concurrent command limit exceeded"
    exit 1
fi
