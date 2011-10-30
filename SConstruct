env = Environment(
    CCFLAGS="-std=c++0x -O3 -g -Wall", 
    LIBS="http-booter", 
    LIBPATH=["out", "."]
)

env.VariantDir("out/src"        , "src")
env.VariantDir("out/test"       , "test")
env.VariantDir("out/benchmarks" , "benchmarks")

env.Library("out/http-booter", [
  "out/src/utils.cc",
  "out/src/http.cc",
  "out/src/http-booter.cc",
  "out/src/buffer.cc",
])

env.Program("out/test-http", [
  "out/test/http.cc"
]);

env.Program("out/test-http-booter", [
  "out/test/http-booter.cc"
]);

env.Program("out/benchmark-raw", [
  "out/benchmarks/raw.cc",
])
