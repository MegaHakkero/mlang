# mlang

A reverse polish based programming language implemented in C++, where parameters always precede everything. Why would you
write this - who knows?
Maybe you just like torturing your soul, eyes and brain. <br>

Am too lazy to write proper docs rn but the basic gist of the program is
```
mlang [-e] [file | expression]
```
If you specify `-e`, the second argument is a single-line expression that you wanna run. If you don't though,
the first argument should be a file that you wanna execute.
If you specify nothing at all, the interpreter will drop you in a REPL session where you can torture yourself as much
as you want, you monster. Look in `test.ml` for a really brief and unclear syntax demo, and in the source file for the
explanations of some of the operators. <br>

Oh yeah, you can compile this with
```
g++ -I. -Wall -std=c++14 -o mlang
```
if you wanna do it the way I do. Probably works with clang too anyways.<br><br>

owo
