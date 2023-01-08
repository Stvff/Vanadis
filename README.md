# Vanadis (better name tbd)

# Improvements
Some of these can be done in parallel, others can not
- Colours in errors
- Better expressions
	- Seperate macro solve step
	- Hexadecimal literals
	- Binary literals
	- Float literals
	- Compile-time figuring out
	- Nesting
- Make sense of the enums and structs
- Custom IO functions (less libc dependent)
- Improve multi-source file imports and inserts
- More support for library functions (dynamic linking?)
- Make more sense of the bytecode
	- Versioned bytecode
	- Expression instructions similar to Instruction instructions
		- conditionals become relative jmps instead of skips (for future code block support?)
	- Decompiler
- jumptables
- label namespaces
- Make the stack and codex make more sense, together with function calls
- Give the user more control over the stack and instruction pointers

# Overview of all instructions
And their argument kinds

## stack operations
|instruction|argument kinds   |name             |
|-----------|-----------------|-----------------|
|`alloc`    |`(datum)`        |allocate         |
|`free`     |`(datum)`        |free             |
|`push`     |`(datum)`        |push             |
|`pop`      |`(mutable page)` |pop              |
|`flip`     |`(datum)`        |flip             |
|`unf`      |`(datum)`        |unflip           |
|`staptr`   |`(datum)`        |get stack pointer|
|`cdxptr`   |`(mutable Datum)`|get codex pointer|


## memory operations
|instruction|argument kinds                            |name       |
|-----------|------------------------------------------|-----------|
|`mov`      |`(mutable page), (page)`                  |move       |
|`set`      |`(mutable datum), (datum)`                |set        |
|`cast`     |`(mutable datum), (datum), (datum)`       |type cast  |
|`memv`     |`(mutable page), (page), (datum), (datum)`|memory move|
|`fill`     |`(mutable page), (datum), (datum)`        |fill       |
|`rsz`      |`(mutable page), (datum)`                 |resize     |

## arithmetic
|instruction|argument kinds                     |name     |
|-----------|-----------------------------------|---------|
|`inc`      |`(mutable datum)`                  |increment|
|`dec`      |`(mutable datum)`                  |decrement|
|`add`      |`(mutable datum), (datum), (datum)`|add      |
|`sub`      |`(mutable datum), (datum), (datum)`|subtract |
|`mul`      |`(mutable datum), (datum), (datum)`|multiply |
|`div`      |`(mutable datum), (datum), (datum)`|divide   |
|`mod`      |`(mutable datum), (datum), (datum)`|modulo   |

## binary operations
|instruction|argument kinds                     |name                 |
|-----------|-----------------------------------|---------------------|
|`and`      |`(mutable datum), (datum), (datum)`|bitwise and          |
|`or`       |`(mutable datum), (datum), (datum)`|bitwise or           |
|`xor`      |`(mutable datum), (datum), (datum)`|bitwise exclusive or |
|`not`      |`(mutable datum), (datum)`         |bitwise inversion    |
|`rshf`     |`(mutable datum), (datum), (datum)`|bitshift to the right|
|`lshf`     |`(mutable datum), (datum), (datum)`|bitshift to the left |

## comparisons
|instruction|argument kinds    |name                            |
|-----------|------------------|--------------------------------|
|`cmp`      |`(datum), (datum)`|compare                         |
|`gcmp`     |`(datum), (datum)`|compare greater than            |
|`scmp`     |`(datum), (datum)`|compare smaller than            |
|`gecmp`    |`(datum), (datum)`|compare greater than or equal to|
|`secmp`    |`(datum), (datum)`|compare smaller than or equal to|
|`pecmp`    |`(page), (page)`  |compage page equal              |

## input and output
|instruction|argument kinds                                    |name        |
|-----------|--------------------------------------------------|------------|
|`input`    |`(mutable page), (page), (datum)`                 |user input  |
|`dprint`   |`(datum), (datum)`                                |digit print |
|`print`    |`(page), (datum)`                                 |page print  |
|`fread`    |`(mutable page), (page), (datum), (mutable datum)`|file read   |
|`fwrite`   |`(page), (page), (datum), (mutable datum)`        |file write  |
|`flen`     |`(mutable datum), (page), (mutable datum)`        |file length |
|`time`     |`(mutable datum)`                                 |current time|

## extra
|instruction|argument kinds      |name      |
|-----------|--------------------|----------|
|`ex`       |`(v), (v), (v), (v)`|expression|
|`lib`      |`(v), (v), (v), (v)`|library   |


## control flow
|instruction|argument     |name                              |
|-----------|-------------|----------------------------------|
|`Ce`       |`(statement)`|condition equal to                |
|`Cs`       |`(statement)`|condition smaller than            |
|`Cg`       |`(statement)`|condition greater than            |
|`Cse`      |`(statement)`|condition smaller than or equal to|
|`Cge`      |`(statement)`|condition greater than or equal to|
|`Cn`       |`(statement)`|condition not equal to            |
|`jmp`      |`(label)`    |jump                              |
|`call`     |`(label)`    |call                              |
|`ret`      |             |return                            |

## macro keywords
|keyword |argument                        |name           |
|--------|--------------------------------|---------------|
|`def`   |`(binding) = (expression)`      |define         |
|`enum`  |`(bindings) = (base expression)`|enumerate      |
|`opens` |                                |open namespace |
|`clons` |                                |close namespace|
|`insert`|`(filename)`                    |insert         |

## expression operators
|operator|preforms                                                |name                    |
|--------|--------------------------------------------------------|------------------------|
|`$`     |`(v)(datum) -- (v)(mutable page)`                       |stack reference         |
|`@`     |`(v)(datum) -- (v)(mutable page)`                       |top-down stack reference|
|`!`     |`(v)(page)  -- (v)(datum)`                              |direct datum            |
|`^`     |`(v)(datum) -- (v)(immutable page)`                     |make page               |
|`]`     |`(page)(datum) -- (page)(datum)`                        |index                   |
|`>`     |`(page)(datum) -- (page)(datum)`                        |relative index          |
|`]*`    |`(mutable page)(datum) -- (mutable page)(mutable datum)`|keeping index           |
|`>*`    |`(mutable page)(datum) -- (mutable page)(mutable datum)`|keeping relative index  |
|`t`     |`(v)(datum) -- (v)(immutable datum)`                    |times size of type      |
|`l`     |`(v)(page)  -- (page)(immutable datum)`                 |length                  |
|`o`     |`(v)(page)  -- (page)(immutable datum)`                 |offset                  |
|`~`     |`(v)(v) -- (v)(v)`                                      |swap                    |
|`,`     |`(v)(v) -- (NULL)(NULL)`                                |comma                   |

# detailed overview
UNFINISHED

## stack operations
### `alloc`
Allocates `d` amount of page of 8 bytes on the stack.

### `free`
Frees `d` amount of pages from the stack.

### `push`
Pushes page `p` to the stack.

### `pop`
Pops the page on the top of the stack into `P`.

### `flip`
Flips `d` amount of pages from the stack to the codex.

### `unf`
Unflips `d` amount of pages from the codex to the stack.

### `staptr`
Sets datum `D` to the value of the stack pointer.

### `cdxptr`
Sets datum `D` to the value of the codex pointer.

## memory operations
### `mov`
Copies page `p` into page `P`.

### `set`
Sets datum `D` to datum `d`.

### `cast`
Casts the second argument from the current type to the type give by the third argument into `D`.

### `memv
