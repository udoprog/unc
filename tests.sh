#!/bin/bash
dir=$1
compiler=$2
shift 2
args=$*

pwd

run_all() {
  for cpp in $dir/*_test.cpp; do
    base=${cpp%%.cpp}
    bin=$base.bin
    deps=$base.flags
    trap "rm -f \"$bin\"" INT QUIT

    echo "running: $base"

    echo "$compiler -g $args $cpp -lboost_test_exec_monitor $(cat $deps) -o $bin"
    $compiler -g $args $cpp -lboost_test_exec_monitor $(cat $deps) -o "$bin"

    if ! time $bin; then
      return 1
    fi
  done

  return 1
}

run_all
exit $?
