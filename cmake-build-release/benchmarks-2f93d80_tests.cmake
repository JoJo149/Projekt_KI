add_test( [==[benchmarks_abgabeBenchmark for generateMoves]==] /Users/jonasmueller/Desktop/Semester_6/Projekt_KI/projekt-ki/cmake-build-release/benchmarks [==[Benchmark for generateMoves]==] --benchmark-samples 10000 --benchmark-warmup-time 100ms  )
set_tests_properties( [==[benchmarks_abgabeBenchmark for generateMoves]==] PROPERTIES WORKING_DIRECTORY /Users/jonasmueller/Desktop/Semester_6/Projekt_KI/projekt-ki/cmake-build-release SKIP_RETURN_CODE 4)
set( benchmarks_TESTS [==[benchmarks_abgabeBenchmark for generateMoves]==])
