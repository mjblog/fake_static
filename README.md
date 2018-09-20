# fake_static
build a single executable(using dynamic link) as if it  were static linked

## How to Use
gcc wrap_execve.c -o wrap.so -shared -O2 -fPIC
export LD_PRELOAD=`pwd`/wrap.so
export __WRAP_LD_PATH=`pwd`/ldwrap

After then, go back to your dir, and run your build as usual.

## Todo
1 try to eliminate the __WRAP_LD_PATH (guess the path from wrap.so)
2 try to eliminate duplicate shared libs in .fakestatic (using a sperated script)