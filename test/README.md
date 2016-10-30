# Testing

## Requiriments
 + [Python 2.7](https://www.python.org/) or [Python 3](https://www.python.org/).
 + The following utilities¹: `sh`, `tar`, `gzip`, `curl`, `md5sum`.
 + [llvm-lit](http://llvm.org/docs/CommandGuide/lit.html)².
 + [OutputCheck](https://github.com/stp/OutputCheck)².

¹: The best way to acquire these utilities on Windows is by installing [MSYS2](https://msys2.github.io/) or [Cygwin](https://www.cygwin.com/). Although, if you have [Git For Windows](https://git-for-windows.github.io/) you probably have MSYS2 already. Just open Git Bash on your start menu and you are ready to go.

²: Once you have Python installed, you can get llvm-lit and OutputCheck by using `pip install lit` and `pip install OutputCheck`. For Windows, `easy_install` instead of `pip` is recommended due to a yet to be fixed bug in `pip` for Windows.

## Running Tests

### Before Running

Open a terminal which has the utilities we need in the enviroment. For Windows that is Git Bash, Cygwin Terminal, or anything else for MSYS2. Don't forget to `cd` into the root directory of this repository.

Then, you need the `GTA3SC` environment variable pointing to the `gta3sc` you want to test. Otherwise, it'll try searching for `gta3sc` on your `PATH`.

You can do this with as little dependencies as possible, in a `sh` compilant shell, by doing the following:

    # The directory may vary, this is the standard for CMake+MSVC.
    export GTA3SC="$(cd "$(dirname ./build/Debug/gta3sc.exe)"; pwd)/$(basename ./build/Debug/gta3sc.exe)"

### Effectively Running Tests

Simply run:

    lit test --verbose
   
If you wish to run a specific test suite, for instance the _codegen_ test suite:

    lit test/codegen --verbose
   
## Writing Tests

Tests are simply `.sc` files (or `.test` files) with one or more `RUN: command` lines specifying what to do in this test. Whenever the `command` fails, the test fails. Nothing more, nothing less.

Of course, since that's the basic premisse of tests, we need additional utilities to help us, thus why we require those base utilities.

Additionally, you can use the following substitutions in a `RUN` line:

| Name      | Description                                                                                                                                                                                                                                                                           |
|-----------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| %s, %t, etc | As explained [here](http://llvm.org/docs/CommandGuide/lit.html#pre-defined-substitutions) on llvm-lit documentation.             
| %(line), %(line+&lt;number&gt;), %(line-&lt;number&gt;) | The number of the line where this substitution is used, with an optional integer offset.  |
| %gta3sc   | Invokes the `gta3sc` being tested.                                                                                                                                                                                                                                                      |
| %not      | Runs its arguments and then inverts the result code from it. 1 becomes 0. Non-1 becomes 0. We use 1 instead of 0 as a base as a form of detecting program crashes, and returning 1 in that case.                                                                                      |
| %dis      | Runs its arguments and discards the result code from it. Unless the called program crashed, in which case it returns 1.                                                                                                                                                               |
| %checksum | Tests if the `md5sum` of `$1` is `$2`.                                                                                                                                                                                                                                                      |
| %verify   | Ensures that `stdin` contains all the errors and warnings specified in the file `$1`. This is a minimal reimplementation of Clang's `-verify` flag, [check here](http://clang.llvm.org/doxygen/classclang_1_1VerifyDiagnosticConsumer.html#details) for details. When piping into this, gta3sc probably needs to be called with `%dis %gta3sc`. |                                                                                                                                                                                                                                         |
| %FileCheck | Ensures that `stdin` matches the content specified in the file `$1`. This is an alias to [OutputCheck](https://github.com/stp/OutputCheck), a reimplementation of LLVM's `FileCheck` . |                                                                                                                                                                                                                                         |


Remember, put related tests into a single file rather than having a separate file per test. Check if there are files already covering your feature and consider adding your code there instead of creating a new file.

If you wish to learn more about the *LLVM Testing Infrastructure*, check out the following links:
  + http://llvm.org/docs/CommandGuide/lit.html
  + http://llvm.org/docs/TestingGuide.html
  + http://clang.llvm.org/doxygen/classclang_1_1VerifyDiagnosticConsumer.html#details
  + https://github.com/llvm-mirror/clang/tree/master/test
  + https://github.com/llvm-mirror/llvm/tree/master/test
  + http://llvm.org/docs/CommandGuide/FileCheck.html
